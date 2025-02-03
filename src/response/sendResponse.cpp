#include "response.hpp"

string	Response::contentTypeHeader() {
	size_t pos = target.rfind(".");
	if (pos == string::npos)
		throw(statusCodeException(501, "Not Implemented"));
	string ext = target.substr(pos);
	string contentTypeValue = getSupportedeExtensions(ext);
	if (contentTypeValue.empty())
		throw(statusCodeException(501, "Not Implemented"));
	return ("Content-Type: " + contentTypeValue + "\r\n");
}

void	Response::sendHeader(const int clientFd) {
	string header;

	header += httpProtocol + " " + to_string(statusCode) + " " + codeMeaning + "\r\n";
	header += contentTypeHeader();
	header += "Transfer-Encoding: chunked\r\n";
	header += "Connection: keep-alive\r\n";
	header += "Server: bngn/0.1\r\n";
	header += "\r\n";
	if (write(clientFd, header.c_str(), header.size()) <= 0) {
		perror("write failed(sendResponse.cpp): ");
		state = CCLOSEDCON;
		return ;
	}
}



void	Response::sendBody(const int clientFd) {
	char buff[BUFFER_SIZE+1] = {0};

	if (contentFd == -1) {
		if ((contentFd = open(target.c_str(), O_RDONLY, 0644)) == -1) {
			perror("open failed(sendresponse.cpp): ");
			throw(statusCodeException(500, "Internal Server Error"));
		}
	}
	int sizeRead;
	if((sizeRead = read(contentFd, buff, BUFFER_SIZE)) < 0) {
		perror("read failed(sendresponse.cpp): ");
		throw(statusCodeException(500, "Internal Server Error"));
	}
	if (sizeRead > 0) {
		ostringstream chunk;
		chunk << hex << sizeRead << "\r\n" << buff << "\r\n";
		if (write(clientFd, chunk.str().c_str(), chunk.str().size()) <= 0) {
			perror("write failed(sendResponse.cpp): ");
			state = CCLOSEDCON;
			return ;
		}
	} else {
		if (write(clientFd, "0\r\n\r\n", 5) <= 0) {
			perror("write failed(sendResponse.cpp): ");
			state = CCLOSEDCON;
			return ;
		}
		state = DONE;
		close(contentFd);
		cerr << "done sending response" << endl;
	}
}