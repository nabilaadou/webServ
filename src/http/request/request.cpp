#include "httpSession.hpp"

httpSession::Request::Request(httpSession& session) : s(session), fd(-1), length(0), requestStat(e_requestStat::headers), remainingBody(NULL, 0) {}

void	httpSession::Request::readfromsock(const int clientFd) {
	char	buffer[BUFFER_SIZE];
	ssize_t byteread;
	ssize_t bufferPos = 0;

	if ((byteread = recv(clientFd, buffer, BUFFER_SIZE, MSG_DONTWAIT)) <= 0) {
		s.sstat = CCLOSEDCON;
		return ;
	}
	bstring bbuffer(buffer, byteread);
	cerr << bbuffer << endl;
	cerr << "---" << endl;
	switch (requestStat)
	{
	case e_requestStat::headers: {
		if ((bufferPos = parseStarterLine(bbuffer)) < 0)
			throw(statusCodeException(400, "Bad Request"));
		if ((bufferPos = s.parseFields(bbuffer, bufferPos, s.headers)) < 0)
			throw(statusCodeException(400, "Bad Request"));
		if (s.sstat == e_sstat::sHeader)
			break;
		requestStat = e_requestStat::bodyFormat;
	}
	case e_requestStat::bodyFormat: {
		bodyFormat();
		requestStat = e_requestStat::handleBody;
	}
	case e_requestStat::handleBody: {
		bbuffer = remainingBody + bbuffer;
		if (bufferPos < bbuffer.size())
			(this->*bodyHandlerFunc)(bbuffer, bufferPos);
	}
	}
}