#include "httpSession.hpp"

httpSession::Response::Response(httpSession& session) : s(session), contentFd(-1), cgiHeadersParsed(false) {}

void	httpSession::Response::sendResponse(const int clientFd) {
	if(s.cgi) {
		if (s.sstat == sHeader) {
			s.cgi->prepearingCgiEnvVars(s.headers);
			s.cgi->setupCGIProcess();
			s.sstat = sBody;
		}
		sendCgiOutput(clientFd);
	} else {
		if (s.sstat == sHeader) {
			sendHeader(clientFd);
			if (s.sstat == CCLOSEDCON)
				return ;
			s.sstat = sBody;
		}
		if (s.method != POST)
			sendBody(clientFd);
		else
			s.sstat = done;
	}
}