#include "httpSession.hpp"

httpSession::Request::Request(httpSession& session) : s(session), fd(-1), requestStat(e_requestStat::headers) {}

void	httpSession::Request::readfromsock(const int clientFd) {
	char	buffer[BUFFER_SIZE];
	ssize_t byteread;
	ssize_t bufferPos;

	if ((byteread = recv(clientFd, buffer, BUFFER_SIZE, MSG_DONTWAIT)) <= 0) {
		s.sstat = CCLOSEDCON;
		return ;
	}
	bstring bbuffer(buffer, byteread);
	cerr << bbuffer << endl;
	switch (requestStat)
	{
	case e_requestStat::headers: {
		if ((bufferPos = parseStarterLine(bbuffer)) < 0)
			throw(statusCodeException(400, "Bad Request"));
		if ((bufferPos = parseFields(bbuffer, bufferPos)) < 0)
			throw(statusCodeException(400, "Bad Request"));
		if (s.sstat == e_sstat::sHeader)
			break;
		requestStat = e_requestStat::body;
	}
	// case e_requestStat::body:
	}
	
	
	// cerr << "m: " << s.method << endl;
	// cerr << "uri: " << s.path << endl;
	// for (const auto& it : s.headers)
	// 	cerr << it.first << ": " << it.second << endl; 
}