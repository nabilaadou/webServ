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
	int		byteRead;
	if ((byteRead =read(clientFd, buffer, BUFFER_SIZE)) <= 0) {
		if (errno == ECONNRESET || !byteRead) {
			state = CCLOSEDCON;
			return;
    	}
		perror("read failed: ");
		throw(statusCodeException(500, "Internal Server Error"));
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