#include "httpSession.hpp"

httpSession::Response::Response(httpSession& session) : s(session), contentFd(-1) {}

void	httpSession::Response::sendResponse(const int clientFd) {
	if (s.cgi == NULL) {
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
	} else {
		if (s.sstat == sHeader) {
			sendCgiStarterLine(clientFd);
			if (s.sstat == CCLOSEDCON)
			return ;
		s.sstat = sBody;
			s.cgi->setupCGIProcess();
		}
		sendCgiOutput(clientFd);
	}
}