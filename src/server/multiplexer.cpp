#include "server.h"

void	resSessionStatus(const int& epollFd, const int& clientFd, map<int, httpSession*>& s, const e_sstat& status) {
	struct epoll_event	ev;

	if (status == done) {
		cerr << "done sending the response" << endl;
		ev.events = EPOLLIN;
		ev.data.fd = clientFd;
		if (epoll_ctl(epollFd, EPOLL_CTL_MOD, clientFd, &ev) == -1) {
			perror("epoll_ctl failed: ");
			throw(statusCodeException(500, "Internal Server Error"));
		}
		delete s[clientFd];
		s.erase(s.find(clientFd));
	}
	else if (status == CCLOSEDCON) {
		cerr << "client closed the connection" << endl;
		if (epoll_ctl(epollFd, EPOLL_CTL_DEL, clientFd, &ev) == -1) {
			perror("epoll_ctl failed: ");
			throw(statusCodeException(500, "Internal Server Error"));//this throw is not supposed to be here
		}
		close(clientFd);
		delete s[clientFd];
		s.erase(s.find(clientFd));
	}
}

void	reqSessionStatus(const int& epollFd, const int& clientFd, map<int, httpSession*>& s, const e_sstat& status) {
	struct epoll_event	ev;

	if (status == sHeader) {
		ev.events = EPOLLOUT;
		ev.data.fd = clientFd;
		if (epoll_ctl(epollFd, EPOLL_CTL_MOD, clientFd, &ev) == -1) {
			perror("epoll_ctl failed: ");
			throw(statusCodeException(500, "Internal Server Error"));
		}
	}
	else if (status == CCLOSEDCON) {
		cerr << "client closed the connection" << endl;
		if (epoll_ctl(epollFd, EPOLL_CTL_DEL, clientFd, &ev) == -1) {
			perror("epoll_ctl failed: ");
			throw(statusCodeException(500, "Internal Server Error"));//this throw is not supposed to be here
		}
		close(clientFd);
		delete s[clientFd];
		s.erase(s.find(clientFd));
	}
}

void	acceptNewClient(const int& epollFd, const int& serverFd) {
	struct epoll_event	ev;
	int					clientFd;

	if ((clientFd = accept(serverFd, NULL, NULL)) < 0) {
		perror("accept faield");
        return;
    }
	ev.events = EPOLLIN;
	ev.data.fd = clientFd;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev) == -1) {
		perror("epoll_ctl faield");
		return;
	}
	cerr << "--------------new client added--------------" << endl;
}

void	multiplexerSytm(const vector<int>& servrSocks, const int epollFd, map<string, configuration>& config) {
	struct epoll_event		events[MAX_EVENTS];
	map<int, httpSession*>	sessions;
	int nfds;

	while (1) {
		cerr << "waiting for requests..." << endl;
		if ((nfds = epoll_wait(epollFd, events, MAX_EVENTS, -1)) == -1) {
			perror("epoll_wait failed");
			if (errno == EBADF || errno == ENOMEM) {
				//send internal error to all clients and close connectiona and reopen epollfd
				// for (map<int, httpSession*>::iterator it = sessions.begin(); it != sessions.end(); ++it) {

				// }
				close(epollFd);
				return;
			}
			continue;
		}
		for (size_t i = 0; i < nfds; ++i) {
			const int fd = events[i].data.fd;
			try {
				if (find(servrSocks.begin(), servrSocks.end(), fd) != servrSocks.end())
					acceptNewClient(epollFd, fd);
				else if (events[i].events & EPOLLIN) {
					sessions.try_emplace(fd, new httpSession(fd, &(config[getsockname(fd)])));
					sessions[fd]->req.readfromsock();
					reqSessionStatus(epollFd, fd, sessions, sessions[fd]->status());
				}
				else if (events[i].events & EPOLLOUT) {
					sessions[fd]->res.sendResponse(fd);
					resSessionStatus(epollFd, fd, sessions, sessions[fd]->status());
				}
			}
			catch (const statusCodeException& exception) {
				if (errorResponse(epollFd, exception, sessions[fd]) < 0)
					sessions.erase(sessions.find(fd));
			}
		}
	}
}