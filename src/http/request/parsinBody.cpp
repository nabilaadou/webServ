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

void	httpSession::Request::contentlength(const bstring& buffer, size_t pos) {
	ssize_t	len = 0;
	size_t	contentStartinPos = pos;
	size_t	size = buffer.size();
	char	ch;
	
	while (pos < size) {
		ch = buffer[pos];
		if (ch == '\n') {
			bool	crInLine = false;

			if (pos && buffer[pos-1] == '\r')
				crInLine = true;
			if (!buffer.ncmp((boundary+"--").c_str(), boundary.size()+2, pos-len)) {
				cerr << "found end boundary" << endl;
				++len;//including pre boundary nl
				if (pos && buffer[pos-len-1] == '\r')
					++len;
				write(fd, &(buffer[contentStartinPos]), pos-contentStartinPos-len);
				if (length - 1)
					throw(statusCodeException(400, "Bad Request6"));
			}
			else if (!buffer.ncmp(boundary.c_str(), boundary.size(), pos-len)) {
				cerr << "------found boundary------" << endl;
				cerr << buffer.substr(pos-len, boundary.size());
				size_t				newPos;
				size_t				boundaryStartinIndex = pos-len;
				map<string, string>	contentHeaders;

				if (fd != -1) {
					++len;//including pre boundary nl
					if (pos && buffer[pos-len-1] == '\r')
						++len;//includin the CR if it exist
					write(fd, &(buffer[contentStartinPos]), pos-contentStartinPos-len);
				}
				s.sstat = e_sstat::emptyline;
				if ((newPos = s.parseFields(buffer, pos+1, contentHeaders)) < 0) {
					cerr << "unfinished body headers" << endl;
					remainingBody = buffer.substr(boundaryStartinIndex);
					length += remainingBody.size();
					fd = -1;
					return;
				}
				s.sstat = e_sstat::body;
				length -= newPos - pos;
				pos = newPos;
				contentStartinPos = newPos;
				fd = openFile(contentHeaders["content-disposition"], s.rules->uploads);
			}
			len = -1;
		}
		if (--length == 0) {
			s.sstat = e_sstat::sHeader; return;
		}
		++len;
		++pos;
	}
	if (len <= boundary.size()+2) {//check if the line is smaller than the boundary
		// cerr << "a potentiel unfinished boundary line" << endl;
		++len;//buggyyyyyyyyyyyyyy
		if (pos-len && pos-len-1 == '\r')
			++len;
		remainingBody = buffer.substr(pos-len);
		length += remainingBody.size();
		write(fd, &(buffer[contentStartinPos]), pos-contentStartinPos-len);
	} else {
		write(fd, &(buffer[contentStartinPos]), pos-contentStartinPos);
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
}
