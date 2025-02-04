#include "request.hpp"

Request::Request() : cgi(NULL), state(PROCESSING){
	parseFunctions.push(&Request::parseBody);
	parseFunctions.push(&Request::parseFileds);
	parseFunctions.push(&Request::parseStartLine);

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
		const auto& func = parseFunctions.top();
		if (!(this->*func)(stream))	return;
		parseFunctions.pop();
	}
	state = DONE;
	cerr << "done parsing the request" << endl;
}