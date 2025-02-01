#include <server.hpp>

void	bngnServer::acceptNewClient(const int serverFd) {
	socklen_t addrlen = sizeof(address);
	int	clientFd;
	if ((clientFd = accept(serverFd, (struct sockaddr*)&address, &addrlen)) < 0) {
        perror("accept"); exit(EXIT_FAILURE);
    }
	fcntl(clientFd, F_SETFL, O_NONBLOCK);
	ev.events = EPOLLIN;
	ev.data.fd = clientFd;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev) == -1) {
		perror("epoll_ctl failed: "); exit(EXIT_FAILURE);
	}
	cerr << "--new client added--" << endl;
}

void	bngnServer::treatRequest(const int clientSock) {
	if (req.getRequestStatus() == false)
		req.parseMessage(clientSock);
	// if (req.getRequestStatus() == true) {
	// 	cerr << "sending the response" << endl;
	// 	int fd = open("../test.txt", O_RDONLY, 0644);
	// 	std::string res = "test";
	// 	write(clientSock, res.c_str(), res.size());
	// 	// send(clientSock, res.c_str(), res.size(), MSG_DONTWAIT);
	// }
}

void	bngnServer::IOMultiplexer() {
	while (1) {
		int nfds;
		cerr << "waiting for requests..." << endl;
		if ((nfds = epoll_wait(epollFd, events, MAX_EVENTS, -1)) == -1) {
			perror("epoll_wait failed: "); exit(-1);
		}
		for (int i = 0; i < nfds; ++i) {
			if (find(listenSockets.begin(), listenSockets.end(), events[i].data.fd) != listenSockets.end()) {
				acceptNewClient(events[i].data.fd);
			}
			else {
				treatRequest(events[i].data.fd);
			}
		}
	}
}
