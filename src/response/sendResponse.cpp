#include "response.hpp"

void	Response::sendStartLine(const int clientFd) {
	string	startLine = httpProtocol + " " + to_string(statusCode) + " " + codeMeaning + "\r\n";
	w_write(clientFd, startLine.c_str(), startLine.size());
	cerr << "sent starter line" << endl;
}

string	Response::contentTypeHeader() {
	size_t pos = target.rfind(".");
	string ext = target.substr(pos);
	string contentTypeValue = getSupportedeExtensions(ext);
	if (contentTypeValue.empty())
		throw(statusCodeException(501, "Not Implemented"));
	return ("Content-Type: " + contentTypeValue);
}

void	Response::sendHeaders(const int clientFd) {
	string	header;

	header = contentTypeHeader();
	w_write(clientFd, header.c_str(), header.size());
	w_write(clientFd, "Transfer-Encoding: chunked\r\n", 28); 
	w_write(clientFd, "Connection: keep-alive\r\n", 24); 
	w_write(clientFd, "Server: bngn/0.1\r\n", 18); 
	w_write(clientFd, "\r\n", 2); 
	cerr << "sent the headers" << endl;
}

void	Response::sendBody(const int clientFd) {
	char buff[BUFFER_SIZE+1] = {0};

	if (contentFd == -1) {
		if ((contentFd = open(target.c_str(), O_RDONLY, 0644)) == -1) {
			perror("open failed: ");
			throw(statusCodeException(500, "Internal Server Error"));
		}
	}
	int sizeRead;
	if((sizeRead = read(contentFd, buff, BUFFER_SIZE)) == -1) {
		perror("read failed: ");
		throw(statusCodeException(500, "Internal Server Error"));
	}
	if (sizeRead) {
		ostringstream sizeInHex;
		sizeInHex << hex << sizeRead << "\r\n";
		w_write(clientFd, sizeInHex.str().c_str(), sizeInHex.str().size());
		w_write(clientFd, buff, sizeRead);
		w_write(clientFd, "\r\n", 2);
	} else {
		w_write(clientFd, "0\r\n\r\n", 5);
		sentAllresponse = true;
		close(contentFd);
		cerr << "done sending response" << endl;
	}
}