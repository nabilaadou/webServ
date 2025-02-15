#include "httpSession.hpp"


bool	getlineFromString(char* buffer, string& line) {
	line  = "";
	while (*buffer && *buffer != '\n') {
		line += *buffer;
		++buffer;
	}
	if (*buffer == 0)
		return false;
	return true;
}

inline std::string& trim(std::string& s) {
	s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
	s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
    return s;
}

int	httpSession::Request::openTargetFile() const {
	int fd;
	if (s.cgi != NULL)
		fd = s.cgi->wFd();
	else if ((fd = open(s.path.c_str(), O_WRONLY | O_CREAT, 0644)) < 0) {
		perror("open failed"); throw(statusCodeException(500, "Internal Server Error"));
	}
	return (fd);
}

bool	httpSession::Request::boundary(char* buffer) {
	string	line;

	if (getlineFromString(buffer, line) && line == boundaryValue)
		return true;
	remainingBuffer = line;
	return false;
}

bool	httpSession::Request::fileHeaders(char* buffer) {
	string			line;

	while(getlineFromString(buffer, line)) {
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
	if (stream.eof()) {
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
	char buff[length+1] = {0};
	stream.read(buff, length);
	while(!parseFunctions.empty()) {
		const auto& func = bodyParseFunctions.front();
		if (!(this->*func)(buff))	return;
		parseFunctions.pop();
	}
	// length -= stream.gcount();
	// write(fd, buff, stream.gcount());
	// if (length == 0) {
	// 	close(fd);
	// 	return true;
	// }
	// return false;
}

bool	httpSession::Request::transferEncodingChunkedBased(stringstream& stream) {
	if (fd == -1)
		fd = openTargetFile();
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


bool	isMultipartFormData(const string& value) {
	vector<string>	fieldValueparts;
	split(value, ';', fieldValueparts);
	if (fieldValueparts.size() != 2)
		return false;
	fieldValueparts[0] = trim(fieldValueparts[0]);
	if (fieldValueparts[0] != "multipart/form-data")
		return false;
	fieldValueparts[1] = trim(fieldValueparts[1]);
	if (strncmp(fieldValueparts[1].c_str(), "boundary=", 9))
		return false;
	return true;
}


bool	httpSession::Request::parseBody(stringstream& stream) {
	if (s.method != "POST")
		return true;
	if (s.headers.find("content-type") != s.headers.end() && isMultipartFormData(s.headers["content-type"])) {
		boundaryValue = s.headers["content-type"].substr(s.headers["content-type"].rfind('=')+1);
		if (s.headers.find("content-length") != s.headers.end())
			parseFunctions.push(&Request::contentLengthBased);
		else if (s.headers.find("transfer-encoding") != s.headers.end() && s.headers["transfer-encoding"] == "chunked")
			parseFunctions.push(&Request::transferEncodingChunkedBased);
	}
	else
		throw(statusCodeException(501, "Not Implemented"));
	return true;
}