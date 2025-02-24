#include "httpSession.hpp"

httpSession::Request::Request(httpSession& session) : s(session) {}

void	httpSession::Request::readfromsock(const int clientFd) {
	char	buffer[BUFFER_SIZE];
	ssize_t byteread;

	if ((byteread = recv(clientFd, buffer, BUFFER_SIZE, MSG_DONTWAIT)) <= 0) {
		s.stat = CCLOSEDCON;
		return ;
	}
	bstring bbuffer(buffer, byteread);
	parseRequest(bbuffer);
	cerr << "m: " << s.method << endl;
	cerr << "uri: " << rawUri << endl;
	for (const auto& it : s.headers) 
		cerr << it.first << ": " << it.second << endl; 
}