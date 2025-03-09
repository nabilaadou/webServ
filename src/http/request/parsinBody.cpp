#include "httpSession.hpp"

static bool	isMultipartFormData(const string& value) {
	vector<string>	fieldValueparts;
	split(value, ';', fieldValueparts);
	if (fieldValueparts.size() != 2)
		return false;
	if (trim(fieldValueparts[0]) != "multipart/form-data")
		return false;
	if (strncmp(trim(fieldValueparts[1]).c_str(), "boundary=", 9))
		return false;
	return true;
}

static int	openFile(const string& value, const string& path) {
	int				fd;
	vector<string>	fieldValueparts;
	vector<string> keyvalue;

	split(value, ';', fieldValueparts);
	if (fieldValueparts.size() != 3 || strncmp("form-data" ,trim(fieldValueparts[0]).c_str(), 9))
		throw(statusCodeException(501, "Not Implemented"));
	split(trim(fieldValueparts[1]), '=', keyvalue);
	if (keyvalue.size() != 2 || strncmp("name", keyvalue[0].c_str(), 4) || keyvalue[1][0] != '"' || keyvalue[1][keyvalue[1].size()-1] != '"')
		throw(statusCodeException(501, "Not Implemented"));
	keyvalue.clear();
	split(trim(fieldValueparts[2]), '=', keyvalue);
	if (keyvalue.size() != 2 || strncmp("filename", keyvalue[0].c_str(), 8) || keyvalue[1][0] != '"' || keyvalue[1][keyvalue[1].size()-1] != '"')
		throw(statusCodeException(501, "Not Implemented"));
	keyvalue[1].erase(keyvalue[1].begin());
	keyvalue[1].erase(keyvalue[1].end()-1);
	if (keyvalue[1].empty())
		throw(statusCodeException(422, "Unprocessable Entity"));
	if ((fd = open((path + "/" + keyvalue[1]).c_str(), O_CREAT | O_WRONLY, 0644)) < 0) {
		perror("open failed");
		throw(statusCodeException(500, "Internal Server Error"));
	}
	return fd;
}

static bool	roundedByNl(const bstring& buffer, const size_t start, const size_t len) {
	// cerr << "is correct boundary" << endl;
	char	ch;

	if (start && buffer[start-1] != '\n')
		return false;
	for (size_t i = start+len; i < buffer.size(); ++i) {
		ch = buffer[i];
		// cerr << (int)ch << endl;
		switch (ch)
		{
		case '\r': {
			if (i != start+len)
				return false;
			break;
		}
		case '\n':
			return i+1;
		default:
			return false;
		}
	}
	return true;
}

void	httpSession::Request::contentlength(const bstring& buffer, size_t pos) {
	size_t	contentStartinPos = pos;

	if (length >= buffer.size())
		length -= buffer.size();
	else {
		cerr << "more content than content length" << endl;
		buffer.substr(0, length);
		length = 0;
	}
	cerr << length << endl;
	while (true) {
		size_t	boundaryStartinPos = buffer.find(boundary.c_str(), pos);
		int		sepBoundary = 0;

		//checking the type of boundary;
		if (boundaryStartinPos == string::npos)
			break;
		if(!buffer.ncmp((boundary+"--").c_str(), boundary.size()+2, boundaryStartinPos)) {
			sepBoundary = 2;
		}
		//check if boundary is rounded by newlines;
		if (roundedByNl(buffer, boundaryStartinPos, boundary.size()+sepBoundary)) {
			if (boundaryStartinPos)
				--boundaryStartinPos;
			if (boundaryStartinPos && buffer[boundaryStartinPos-1] == '\r')
				--boundaryStartinPos;
			if (sepBoundary == 0) {
				map<string, string>	contentHeaders;
				if (fd != -1)
					write(fd, &(buffer[contentStartinPos]), boundaryStartinPos-contentStartinPos);
				s.sstat = e_sstat::emptyline;
				if ((contentStartinPos = s.parseFields(buffer, buffer.find('\n', boundaryStartinPos+boundary.size())+1, contentHeaders)) < 0) {
					cerr << "unfinished body headers" << endl;
					remainingBody = buffer.substr(boundaryStartinPos);
					length += remainingBody.size();
					fd = -1;
					return;
				}
				s.sstat = e_sstat::body;
				fd = openFile(contentHeaders["content-disposition"], s.rules->uploads);
			} else {
				cerr << "end boundary" << endl;
				write(fd, &(buffer[contentStartinPos]), boundaryStartinPos-contentStartinPos);
				if (length)
					throw(statusCodeException(400, "Bad Request"));
			}
		}
		pos = boundaryStartinPos+boundary.size();
	}
	size_t lastlinePos = buffer.rfind('\n');
	if (length == 0)
		s.sstat = e_sstat::sHeader;
	else if (buffer.size()-lastlinePos <= boundary.size()+2) {
		if (lastlinePos && buffer[lastlinePos-1] == '\r')
			--pos;
		remainingBody = buffer.substr(lastlinePos);
		length += remainingBody.size();
		write(fd, &(buffer[contentStartinPos]), lastlinePos);
	} else {
		write(fd, &(buffer[contentStartinPos]), buffer.size()-contentStartinPos);
	}
}

void	httpSession::Request::unchunkBody(const bstring& buffer, size_t pos) {
	size_t size = buffer.size();

	while (pos < size) {
		stringstream	ss;
		bool			crInLine = false;
		size_t			nlPos;
	
		// if (length == 0) {//dones;t wrk
			nlPos = buffer.find('\n', pos);
			if (nlPos != string::npos) {
				if (nlPos && buffer[nlPos-1] == '\r')
					crInLine = true;
				string hexLength = buffer.substr(pos, nlPos-pos-crInLine).cppstring();//incase of unvalid number then whatttttttttt;
				if (hexLength == "0") {
					s.headers["content-length"] = to_string(s.cgiBody.size());
					s.headers.erase(s.headers.find("transfer-encoding"));
					s.sstat = e_sstat::sHeader;
					cerr << "cgi's body(unchunked)" << endl;
					cerr << s.cgiBody << endl;
					cerr << "------------------------------------" << endl;
					return ;
				}
				ss << hex << hexLength;
				ss >> length;
			} else {
				remainingBody = buffer.substr(pos);
				return;
			}
			pos = nlPos+1;
		// }
		nlPos = buffer.find('\n', pos+length);
		if (nlPos != string::npos) {
			s.cgiBody += buffer.substr(pos, length);
			// length = 0;
			pos = nlPos;//so i can start next iteration from the line that has the content
		} else {
			remainingBody = buffer.substr(pos+1);
			return;
		}
		++pos;
	}
}

void	httpSession::Request::bufferTheBody(const bstring& buffer, size_t pos) {
	if (length) {
		s.cgiBody += buffer.substr(pos);
		length -= buffer.size()-pos;
	}
	if (length == 0) {
		cerr << "cgi's body" << endl;
		cerr << s.cgiBody << endl;
		s.sstat = e_sstat::sHeader;
		cerr << "--------" << endl;
	}
}

void	httpSession::Request::bodyFormat() {
	if (s.cgi) {
		if (s.headers.find("content-length") != s.headers.end()) {
			length = w_stoi(s.headers["content-length"]);
			bodyHandlerFunc = &Request::bufferTheBody;
		}
		else if (s.headers.find("transfer-encoding") != s.headers.end() && s.headers["transfer-encoding"] == "chunked")
			bodyHandlerFunc = &Request::unchunkBody;
	}
	else if (s.headers.find("content-type") != s.headers.end() && isMultipartFormData(s.headers["content-type"])) {
		boundary = "--" + s.headers["content-type"].substr(s.headers["content-type"].rfind('=')+1);
		if (s.headers.find("content-length") != s.headers.end()) {
			length = w_stoi(s.headers["content-length"]);
			bodyHandlerFunc = &Request::contentlength;
		}
		// else if (s.headers.find("transfer-encoding") != s.headers.end() && s.headers["transfer-encoding"] == "chunked")
		// 	bodyHandlerFunc = &Request::contentlength;
	} else
		throw(statusCodeException(501, "Not Implemented"));
	s.statusCode = 204;
	s.codeMeaning = "No Content";
}
