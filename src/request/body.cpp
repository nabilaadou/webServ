#include "httpSession.hpp"

bool	getlineFromString(string& buffer, string& line) {
	line  = "";
	int i = 0;
	while (buffer[i] && buffer[i] != '\n') {
		line += buffer[i];
		++i;
	}
	if (buffer[i] == 0) {
		buffer.erase(buffer.begin(), buffer.begin()+i);
		return true;
	}
	buffer.erase(buffer.begin(), buffer.begin()+i+1);
	return false;
}

int	httpSession::Request::openTargetFile(const string& filename) const {
	int fd;
	if (s.cgi != NULL)
		fd = s.cgi->wFd();
	else if ((fd = open(filename.c_str(), O_CREAT | O_WRONLY, 0644)) < 0) {
		perror("open failed"); throw(statusCodeException(500, "Internal Server Error"));
	}
	return (fd);
}

bool	httpSession::Request::boundary(string& buffer) {
	string	line;

	if (getlineFromString(buffer, line) && line == boundaryValue)
		return true;
	remainingBuffer = line;
	return false;
}

bool	httpSession::Request::fileHeaders(string& buffer) {
	string	line;
	bool	eof;

	while((eof = getlineFromString(buffer, line)) == false && line != " " && !line.empty()) {
		string	fieldName;
		string	filedValue;

		if (!contentHeaders.empty() && (line[0] == ' ' || line[0] == '\t')) {
			contentHeaders[prvsContentFieldName] += " " + trim(line);
			continue ;
		}
		size_t colonIndex = line.find(':');
		fieldName = line.substr(0, colonIndex);
		if (colonIndex != string::npos && colonIndex+1 < line.size()) {
			filedValue = line.substr(colonIndex+1);
			filedValue = trim(filedValue);
		}
		if (colonIndex == string::npos || !validFieldName(fieldName))
			throw(statusCodeException(400, "Bad Request"));
		contentHeaders[fieldName] = filedValue;
		prvsContentFieldName = fieldName;
	}
	if (eof) {
		remainingBuffer = line;
		return false;
	}
	return true;
}

static string	retrieveFilename(const string& value) {
	vector<string>	fieldValueparts;
	split(value, ';', fieldValueparts);
	for (const auto& it : fieldValueparts)
		cerr << it << endl;
	if (fieldValueparts.size() != 3)
		return "UNVALID1";
	if (strncmp("form-data" ,trim(fieldValueparts[0]).c_str(), 9))
		return "UNVALID2";
	vector<string> keyvalue;
	split(trim(fieldValueparts[1]), '=', keyvalue);
	if (keyvalue.size() != 2 || strncmp("name", keyvalue[0].c_str(), 4) || keyvalue[1][0] != '"' || keyvalue[1][keyvalue[1].size()-1] != '"')
		return "UNVALID3";
	keyvalue.clear();
	split(trim(fieldValueparts[2]), '=', keyvalue);
	if (keyvalue.size() != 2 || strncmp("filename", keyvalue[0].c_str(), 8) || keyvalue[1][0] != '"' || keyvalue[1][keyvalue[1].size()-1] != '"')
		return "UNVALID4";
	keyvalue[1].erase(keyvalue[1].begin());
	keyvalue[1].erase(keyvalue[1].end()-1);
	return keyvalue[1];
}

bool	httpSession::Request::fileContent(string& buffer) {
	string	line;
	bool	eof;

	if (fd == -1) {
		if (s.headers.find("content-disposition") != s.headers.end()) {
			string filename = retrieveFilename(s.headers["content-disposition"]);
			fd = openTargetFile(filename);
		} else
			throw(statusCodeException(501, "Not Implemented"));
	}
	while ((eof = getlineFromString(buffer, line)) == false) {
		if (line == boundaryValue) {
			fd = -1;
			bodyParseFunctions.push(&Request::fileHeaders);
			bodyParseFunctions.push(&Request::fileContent);
			break ;
		} else if (line == boundaryValue + "--") {
			break ;
		}
		if (write(fd, line.c_str(), line.size()) <= 0) {
			perror("wirte failed(body.cpp 102)");
        	throw(statusCodeException(500, "Internal Server Error"));
		}
	}
	if (eof) {
		remainingBuffer = line;
		return false;
	}
	return true;
}

bool	httpSession::Request::contentLengthBased(stringstream& stream) {
	if (!length) {
		try {
			length = stoi(s.headers["content-length"]);
		} catch(...) {
			perror("unvalid number in content length"); throw(statusCodeException(500, "Internal Server Error"));
		}
	}
	cerr << length << endl;
	char buff[length+1] = {0};
	stream.read(buff, length);
	string	stringBuff = string(buff);
	while(!parseFunctions.empty()) {
		const auto& func = bodyParseFunctions.front();
		if (!(this->*func)(stringBuff))	break;
		parseFunctions.pop();
	}
	length -= stream.gcount() - remainingBuffer.size();
	return (length == 0) ? true : false;
}

bool	httpSession::Request::transferEncodingChunkedBased(stringstream& stream) {
	if (fd == -1)
		fd = openTargetFile("test");
	while (1) {
		string	line;
		if (length <= 0) {
			if (getline(stream, line)) {
				if (stream.eof()) {
					remainingBuffer = line;
					return false;
				}
				perror("getline failed"); throw(statusCodeException(500, "Internal Server Error"));
			}
			try {
				length = stoi(line);
			}
			catch(...) {
				perror("unvalid number in chunked length"); throw(statusCodeException(500, "Internal Server Error"));
			}
			if (line == "0") {
				close(fd);
				return true;
			}
		}
		char buff[length+1] = {0};
		stream.read(buff, length);
		if (stream.gcount() == 0) return false;
		length -= stream.gcount();
		write(fd, buff, stream.gcount());
		getline(stream, line); // consume the \n(its not included n the length)
	}
}


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


bool	httpSession::Request::parseBody(stringstream& stream) {
	if (s.method != "POST")
		return true;
	if (s.headers.find("content-type") != s.headers.end() && isMultipartFormData(s.headers["content-type"])) {
		boundaryValue = "--" + s.headers["content-type"].substr(s.headers["content-type"].rfind('=')+1);
		if (s.headers.find("content-length") != s.headers.end())
			parseFunctions.push(&Request::contentLengthBased);
		else if (s.headers.find("transfer-encoding") != s.headers.end() && s.headers["transfer-encoding"] == "chunked")
			parseFunctions.push(&Request::transferEncodingChunkedBased);
	}
	else
		throw(statusCodeException(501, "Not Implemented"));
	return true;
}