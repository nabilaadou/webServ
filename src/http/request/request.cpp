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
		if (s.cgi)
			s.cgi->prepearingCgiEnvVars(s.headers);
		if (s.sstat == e_sstat::sHeader)
			break;
		requestStat = e_requestStat::body;
	}
	case e_requestStat::body: {
		bbuffer = remainingBody + bbuffer;
		if (bufferPos < bbuffer.size())
			parseBody(bbuffer, bufferPos);
	}
	}
}

/*
	- body
	- header -> content-length or chunked
	- cgi -> in case of chunked unchunk the body

	body;
	
*/