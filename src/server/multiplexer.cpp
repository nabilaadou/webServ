#include "server.h"

void	resSessionStatus(const int& epollFd, const int& clientFd, t_httpSession& session, const t_state& status) {
	struct epoll_event	ev;

	if (session.res.responseStatus() == DONE) {
		ev.events = EPOLLIN;
		ev.data.fd = clientFd;
		if (epoll_ctl(epollFd, EPOLL_CTL_MOD, clientFd, &ev) == -1) {
			perror("epoll_ctl failed: ");
			throw(statusCodeException(500, "Internal Server Error"));
		}
		session.res = Response();
	}
	else if (session.res.responseStatus() == CCLOSEDCON) {
		if (epoll_ctl(epollFd, EPOLL_CTL_DEL, clientFd, &ev) == -1) {
			perror("epoll_ctl failed: ");
			throw(statusCodeException(500, "Internal Server Error"));//this throw is not supposed to be here
		}
		close(clientFd);
		session.res = Response();
	}
}

void	reqSessionStatus(const int& epollFd, const int& clientFd, t_httpSession& session, const t_state& status) {
	struct epoll_event	ev;

	if (session.req.RequestStatus() == DONE) {
		ev.events = EPOLLOUT;
		ev.data.fd = clientFd;
		if (epoll_ctl(epollFd, EPOLL_CTL_MOD, clientFd, &ev) == -1) {
			perror("epoll_ctl failed: ");
			throw(statusCodeException(500, "Internal Server Error"));
		}
		session.res.equipe(session.req.Method(), session.req.Path(), session.req.HttpProtocole());
		session.req = Request();
	}
	else if (session.req.RequestStatus() == CCLOSEDCON) {
		if (epoll_ctl(epollFd, EPOLL_CTL_DEL, clientFd, &ev) == -1) {
			perror("epoll_ctl failed: ");
			throw(statusCodeException(500, "Internal Server Error"));//this throw is not supposed to be here
		}
		close(clientFd);
		session.req = Request();
	}
}

void	acceptNewClient(const int& epollFd, const int& serverFd, const t_sockaddr& addrsInfo) {
	struct epoll_event	ev;
	int					clientFd;
	socklen_t			addrsLen = sizeof(addrsInfo);

	if ((clientFd = accept(serverFd, (struct sockaddr*)&addrsInfo, &addrsLen)) < 0) {
		perror("accept faield: ");
        throw(statusCodeException(500, "Internal Server Error"));//i can't send the error page//no fd to send to
    }
	ev.events = EPOLLIN;
	ev.data.fd = clientFd;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev) == -1) {
		perror("epoll_ctl faield(setUpserver.cpp): ");
		throw(statusCodeException(500, "Internal Server Error"));
	}
	cerr << "-------new client added-------" << endl;
}

void	multiplexerSytm(map<int, t_sockaddr>& servrSocks, const int& epollFd) {
	struct epoll_event		events[MAX_EVENTS];
	map<int, t_httpSession>	sessions;

	while (1) {
		int nfds;
		cerr << "waiting for requests..." << endl;
		if ((nfds = epoll_wait(epollFd, events, MAX_EVENTS, -1)) == -1) {
			//send the internal error page to all current clients
			//close all connections and start over
			perror("epoll_wait failed(setUpserver.cpp): ");
		}
		for (int i = 0; i < nfds; ++i) {
			const int fd = events[i].data.fd;
			try {
				if (servrSocks.find(fd) != servrSocks.end())
					acceptNewClient(epollFd, fd, servrSocks[fd]);
				else if (events[i].events & EPOLLIN) {
					sessions[fd].req.parseMessage(fd);
					reqSessionStatus(epollFd, fd,sessions[fd], sessions[fd].req.RequestStatus());
				}
				else if (events[i].events & EPOLLOUT) {
					sessions[fd].res.sendResponse(fd);
					resSessionStatus(epollFd, fd,sessions[fd], sessions[fd].res.responseStatus());
				}
			}
			catch (const statusCodeException& exception) {
				cerr << "code--> " << exception.code() << endl;
				cerr << "reason--> " << exception.meaning() << endl;
				//send error page;
				exit(-1);
			}
		}
	}
}