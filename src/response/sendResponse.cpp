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
		perror("write failed(sendResponse.cpp 24)");
		state = CCLOSEDCON_S;
		return ;
	}
}



void	Response::sendBody(const int clientFd) {
	char buff[BUFFER_SIZE+1] = {0};

	if (contentFd == -1) {
		if ((contentFd = open(target.c_str(), O_RDONLY, 0644)) == -1) {
			perror("open failed(sendresponse.cpp 37)");
			throw(statusCodeException(500, "Internal Server Error"));
		}
	}
	int sizeRead;
	if((sizeRead = read(contentFd, buff, BUFFER_SIZE)) < 0) {
		perror("read failed(sendresponse.cpp 43)");
		throw(statusCodeException(500, "Internal Server Error"));
	}
	if (sizeRead > 0) {
		ostringstream chunkSize;
		chunkSize << hex << sizeRead << "\r\n";
		if (write(clientFd, chunkSize.str().c_str(), chunkSize.str().size()) <= 0) {
			perror("write failed(sendResponse.cpp 50)");
			state = CCLOSEDCON_S;
			return ;
		}
		if (write(clientFd, buff, sizeRead) <= 0) {
			perror("write failed(sendResponse.cpp 50)");
			state = CCLOSEDCON_S;
			return ;
		}
		if (write(clientFd, "\r\n", 2) <= 0) {
			perror("write failed(sendResponse.cpp 50)");
			state = CCLOSEDCON_S;
			return ;
		}
	} else {
		if (write(clientFd, "0\r\n\r\n", 5) <= 0) {
			perror("write failed(sendResponse.cpp 56)");
			state = CCLOSEDCON_S;
			return ;
		}
		state = DONE_S;
		close(contentFd);
		cerr << "done sending response" << endl;
	}
}