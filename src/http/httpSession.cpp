#include "httpSession.hpp"

httpSession::httpSession(int clientFd, configuration* config)
	: clientFd(clientFd), config(config), req(Request(*this)), res(Response(*this))
	, sstat(e_sstat::method), cgi(NULL), rules(NULL), statusCode(200)
	, codeMeaning("OK") {}

httpSession::httpSession()
	: clientFd(clientFd), config(NULL), req(Request(*this)), res(Response(*this))
	, cgi(NULL), sstat(e_sstat::method), statusCode(200), codeMeaning("OK") {}

configuration*	httpSession::clientConfiguration() const {
	return config;
}

int	httpSession::fd() const {
	return clientFd;
}

const e_sstat& httpSession::status() const {
	return sstat;
}

void	httpSession::reSetPath(const string& newPath) {
	path = newPath;
}