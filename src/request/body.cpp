#include "httpSession.hpp"

int	httpSession::Request::openTargetFile() const {
	int fd;
	if (s.cgi != NULL)
		fd = s.cgi->wFd();
	else if ((fd = open(s.path.c_str(), O_WRONLY | O_CREAT, 0644)) < 0) {
		perror("open failed"); throw(statusCodeException(500, "Internal Server Error"));
	}
	return (fd);
}

bool	httpSession::Request::contentLengthBased(stringstream& stream) {
	if (fd == -1)
		fd = openTargetFile();
	if (!length) {
		try {
			length = stoi(s.headers["content-length"]);
		}
		catch(...) {
			perror("unvalid number in content length"); throw(statusCodeException(500, "Internal Server Error"));
		}
	}
	char buff[length+1] = {0};
	stream.read(buff, length);
	length -= stream.gcount();
	write(fd, buff, stream.gcount());
	if (length == 0) {
		close(fd);
		return true;
	}
	return false;
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



bool	httpSession::Request::parseBody(stringstream& stream) {
	if (s.method != "POST")
		return true;
	if (s.headers.find("content-length") != s.headers.end())
		parseFunctions.push(&Request::contentLengthBased);
	else if (s.headers.find("transfer-encoding") != s.headers.end() && s.headers["transfer-encoding"] == "chunked")
		parseFunctions.push(&Request::transferEncodingChunkedBased);
	else
		throw(statusCodeException(501, "Not Implemented"));
	return true;
}