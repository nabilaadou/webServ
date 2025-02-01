#include <server.hpp>

void	bngnServer::acceptNewClient(const int serverFd) {
	socklen_t addrlen = sizeof(address);
	int	clientFd;
	if ((clientFd = accept(serverFd, (struct sockaddr*)&address, &addrlen)) < 0) {
        perror("accept"); exit(EXIT_FAILURE);
    }
	ev.events = EPOLLIN;
	ev.data.fd = clientFd;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev) == -1) {
		perror("epoll_ctl failed: "); exit(EXIT_FAILURE);
	}
	cerr << "--new client added--" << endl;
}

void	bngnServer::IOMultiplexer() {
	while (1) {
		int nfds;
		cerr << "waiting for requests..." << endl;
		if ((nfds = epoll_wait(epollFd, events, MAX_EVENTS, -1)) == -1) {
			perror("epoll_wait failed: "); exit(-1);
		}
		try {
			for (int i = 0; i < nfds; ++i) {
				if (find(listenSockets.begin(), listenSockets.end(), events[i].data.fd) != listenSockets.end()) {
					acceptNewClient(events[i].data.fd);
				}
				else if (events[i].events & EPOLLIN){
					httpSessions[events[i].data.fd].req.parseMessage(events[i].data.fd);
					if (httpSessions[events[i].data.fd].req.getRequestStatus() == true) {
						ev.events = EPOLLOUT;
						ev.data.fd = events[i].data.fd;
						if (epoll_ctl(epollFd, EPOLL_CTL_MOD, events[i].data.fd, &ev) == -1) {
							perror("epoll_ctl failed: "); exit(EXIT_FAILURE);
						}
					}
				}
				else if (events[i].events & EPOLLOUT) {
					// httpSessions[events[i].data.fd].res
				}
			}
		}
		catch (const statusCodeException& exception) {
			//call appropiete function
		}
	}
}
