#include "httpSession.hpp"

httpSession::Request::Request(httpSession& session) : s(session), fd(-1), bodyFormat(-1) {}

void	httpSession::Request::readfromsock(const int clientFd) {
	char	buffer[BUFFER_SIZE];
	ssize_t byteread;
	ssize_t fieldlinesPos;

	if ((byteread = recv(clientFd, buffer, BUFFER_SIZE, MSG_DONTWAIT)) <= 0) {
		s.sstat = CCLOSEDCON;
		return ;
	}
	bstring bbuffer(buffer, byteread);
	if ((fieldlinesPos = parseStarterLine(bbuffer)) < 0)
		throw(statusCodeException(400, "Bad Request"));
	
	
	// cerr << "m: " << s.method << endl;
	// cerr << "uri: " << s.path << endl;
	// for (const auto& it : s.headers) 
	// 	cerr << it.first << ": " << it.second << endl; 
}