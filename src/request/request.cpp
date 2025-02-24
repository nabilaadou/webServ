#include "httpSession.hpp"

httpSession::Request::Request(httpSession& session) : s(session), stat(e_sstat::method) {}

void	httpSession::Request::readfromsock(const int clientFd) {
	char	buffer[BUFFER_SIZE];
	ssize_t byteread;

	if ((byteread = recv(clientFd, buffer, BUFFER_SIZE, MSG_DONTWAIT)) <= 0) {
		return ;
	}
	parseRequest(buffer, byteread);
}