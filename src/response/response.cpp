#include "response.hpp"

Response::Response(): statusCode(200), codeMeaning("OK"), contentFd(-1), state(PROCESSING), cgi(NULL) {}

void	Response::sendResponse(const int clientFd) {
	if (cgi == NULL) {
		if (state == PROCESSING) {
			sendHeader(clientFd);
			if (state == CCLOSEDCON)
				return ;
			state = SHEADER;
		}
		sendBody(clientFd);
	} else {
		if (state == PROCESSING) {
			sendCgiStarterLine(clientFd);
			if (state == CCLOSEDCON)
				return ;
			state = SHEADER;
			cgi->setupCGIProcess();
		}
		sendCgiOutput(clientFd);
	}
}

void	Response::equipe(const string& methode, const string& path, const string& http, Cgi* cgi) {
	this->methode = methode;
	this->target = path;
	this->httpProtocol = http;
	this->cgi = cgi;
}

void	Response::setStatusCode(const int code, const string& meaning) {
	this->statusCode = code;
	this->codeMeaning = meaning;
}

const t_state&	Response::responseStatus() const{
	return state;
}

