#include "httpSession.hpp"

httpSession::Request::Request(httpSession& session) : s(session), state(PROCESSING), length(0), fd(-1) {
	parseFunctions.push(&Request::parseStartLine);
	parseFunctions.push(&Request::parseFileds);
	// parseFunctions.push(&Request::parseBody);

	bodyParseFunctions.push(&Request::boundary);
	bodyParseFunctions.push(&Request::fileHeaders);
	bodyParseFunctions.push(&Request::fileContent);
}

void	httpSession::Request::parseMessage(const int clientFd) {
	char	buffer[BUFFER_SIZE+1] = {0};

	if ((byteread = read(clientFd, buffer, BUFFER_SIZE)) <= 0) {
		state = CCLOSEDCON;
		return;
	}
	// remainingBuffer += buffer;
	// replace(remainingBuffer.begin(), remainingBuffer.end(), '\r', ' ');
	// stringstream	stream(remainingBuffer);
	// remainingBuffer.clear();
	while(!parseFunctions.empty()) {
		const auto& func = parseFunctions.front();
		if (!(this->*func)(buffer))	return;
		parseFunctions.pop();
	}
	state = DONE;
	cerr << "done parsing the request" << endl;
}

const t_state& httpSession::Request::status() const {
	return state;
}