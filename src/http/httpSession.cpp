#include "httpSession.hpp"

httpSession::httpSession(int clientFd, configuration* config)
	: config(config), req(Request(*this)), res(Response(*this))
	, sstat(e_sstat::method), cgi(NULL), rules(NULL), statusCode(200)
	, codeMeaning("OK"), body(NULL, 0) {}

httpSession::httpSession()
	: config(NULL), req(Request(*this)), res(Response(*this))
	, cgi(NULL), sstat(e_sstat::method), statusCode(200), codeMeaning("OK")
	, body(NULL, 0) {}

const e_sstat& httpSession::status() const {
	return sstat;
}

void	httpSession::reSetPath(const string& newPath) {
	path = newPath;
}