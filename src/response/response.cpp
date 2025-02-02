#include "response.hpp"

Response::Response(): statusCode(200), codeMeaning("OK"), contentFd(-1) {}

void	Response::sendResponse(const int clientFd) {
	if (sentHeader == false) {
		sendStartLine(clientFd);
		sendHeaders(clientFd);
		sentHeader = true;
	}
	sendBody(clientFd);
}

void	Response::equipe(const string& methode, const string& path, const string& http) {
	this->methode = methode;
	this->target = path;
	this->httpProtocol = http;
}

void	Response::setStatusCode(const int code, const string& meaning) {
	this->statusCode = code;
	this->codeMeaning = meaning;
}

bool	Response::getResponseStatus() {
	return sentAllresponse;
}

