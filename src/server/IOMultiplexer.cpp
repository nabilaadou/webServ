#include <server.hpp>

void	bngnServer::acceptNewClient(const int serverFd) {
	socklen_t addrlen = sizeof(address);
	int	clientFd;
	if ((clientFd = accept(serverFd, (struct sockaddr*)&address, &addrlen)) < 0) {
		perror("accept faield: ");
        throw(statusCodeException(500, "Internal Server Error"));
    }
	ev.events = EPOLLIN;
	ev.data.fd = clientFd;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev) == -1) {
		perror("epoll_ctl faield: ");
		throw(statusCodeException(500, "Internal Server Error"));
	}
	cerr << "-------new client added-------" << endl;
}

void	bngnServer::IOMultiplexer() {
	while (1) {
		int nfds;
		cerr << "waiting for requests..." << endl;
		if ((nfds = epoll_wait(epollFd, events, MAX_EVENTS, -1)) == -1) {
			perror("epoll_wait failed: "); exit(-1);
		}
		for (int i = 0; i < nfds; ++i) {
			try {
				if (find(listenSockets.begin(), listenSockets.end(), events[i].data.fd) != listenSockets.end()) {
					acceptNewClient(events[i].data.fd);
				}
				else if (events[i].events & EPOLLIN){
					httpSessions[events[i].data.fd].req.parseMessage(events[i].data.fd);
					if (httpSessions[events[i].data.fd].req.getRequestStatus() == DONE) {
						ev.events = EPOLLOUT;
						ev.data.fd = events[i].data.fd;
						if (epoll_ctl(epollFd, EPOLL_CTL_MOD, events[i].data.fd, &ev) == -1) {
							perror("epoll_ctl failed: ");
							throw(statusCodeException(500, "Internal Server Error"));
						}
						const Request& tmpReq = httpSessions[events[i].data.fd].req;
						httpSessions[events[i].data.fd].res.equipe(tmpReq.getMethod(), tmpReq.getPath(), tmpReq.getHttpProtocole());
						httpSessions[events[i].data.fd].req = Request();
					}
					else if (httpSessions[events[i].data.fd].req.getRequestStatus() == CCLOSEDCON) {
						if (epoll_ctl(epollFd, EPOLL_CTL_DEL, events[i].data.fd, &ev) == -1) {
							perror("epoll_ctl failed: ");
							throw(statusCodeException(500, "Internal Server Error"));
						}
						close(events[i].data.fd);
					}
				}
				else if (events[i].events & EPOLLOUT) {
					httpSessions[events[i].data.fd].res.sendResponse(events[i].data.fd);
					if (httpSessions[events[i].data.fd].res.getResponseStatus() == true) {
						ev.events = EPOLLIN;
						ev.data.fd = events[i].data.fd;
						if (epoll_ctl(epollFd, EPOLL_CTL_MOD, events[i].data.fd, &ev) == -1) {
							perror("epoll_ctl failed: ");
							throw(statusCodeException(500, "Internal Server Error"));
						}
						httpSessions[events[i].data.fd].res = Response();
					}
				}
			}
			catch (const statusCodeException& exception) {
				cerr << "code--> " << exception.code() << endl;
				cerr << "reason--> " << exception.meaning() << endl;
				exit(-1);
			}
		}
		}
}
