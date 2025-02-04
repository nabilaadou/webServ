#include "response.hpp"

Response::Response(): statusCode(200), codeMeaning("OK"), contentFd(-1), state(PROCESSING) {}

void	Response::sendResponse(const int clientFd) {
	if (state == PROCESSING) {
		sendHeader(clientFd);
		if (state == CCLOSEDCON)
			return ;
		state = SHEADER;
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

const t_state&	Response::responseStatus() const{
	return state;
}

