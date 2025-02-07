#include "request.hpp"

Request::Request() : cgi(NULL), state(PROCESSING), length(0), fd(-1) {
	parseFunctions.push(&Request::parseStartLine);
	parseFunctions.push(&Request::parseFileds);
	parseFunctions.push(&Request::parseBody);

	parseFunctionsStarterLine.push(&Request::isProtocole);
	parseFunctionsStarterLine.push(&Request::isTarget);
	parseFunctionsStarterLine.push(&Request::isMethod);
}


void	Request::parseMessage(const int clientFd) {
	char	buffer[BUFFER_SIZE+1] = {0};

	if (read(clientFd, buffer, BUFFER_SIZE) <= 0) {
		state = CCLOSEDCON;
		return;
	}
	remainingBuffer += buffer;
	replace(remainingBuffer.begin(), remainingBuffer.end(), '\r', ' ');
	stringstream	stream(remainingBuffer);
	while(!parseFunctions.empty()) {
		const auto& func = parseFunctions.front();
		if (!(this->*func)(stream))	return;
		parseFunctions.pop();
	}
	state = DONE;
	cerr << "done parsing the request" << endl;
}