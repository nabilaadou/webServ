#include "httpSession.hpp"

void	sendError(const int clientFd, const int statusCode, const string codeMeaning) {
	string msg;
	msg += "HTTP/1.1 " + to_string(statusCode) + " " + codeMeaning + "\r\n"; 
	msg += "Content-type: text/html\r\n";
	msg += "Transfer-Encoding: chunked\r\n";
	msg += "Connection: keep-alive\r\n";
	msg += "Server: bngn/0.1\r\n";
	msg += "\r\n";
	write(clientFd, msg.c_str(), msg.size());
	string body = "<!DOCTYPE html><html><body><h1>" + codeMeaning + "</h1></body></html>";
	ostringstream chunkSize;
	chunkSize << hex << body.size() << "\r\n";
	write(clientFd, chunkSize.str().c_str(), chunkSize.str().size());
	write(clientFd, body.c_str(), body.size());
	write(clientFd, "\r\n", 2);
	write(clientFd, "0\r\n\r\n", 5);
}
