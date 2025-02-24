#include "httpSession.hpp"

httpSession::Response::Response(httpSession& session) : s(session), contentFd(-1) {}

void	httpSession::Response::sendResponse(const int clientFd) {
	if (s.cgi == NULL) {
		if (s.stat == sHeader) {
			sendHeader(clientFd);
			if (s.stat == CCLOSEDCON)
				return ;
			s.stat = sBody;
		}
		if (s.method != POST)
			sendBody(clientFd);
		else
			s.stat = done;
	} else {
		if (s.stat == sHeader) {
			sendCgiStarterLine(clientFd);
			if (s.stat == CCLOSEDCON)
			return ;
		s.stat = sBody;
			s.cgi->setupCGIProcess();
		}
		sendCgiOutput(clientFd);
	}
}