#include "server.hpp"

bngnServer::bngnServer() {
	startServer();
	createEpollInstance();
	addListenSocksToEpoll();
	IOMultiplexer();
}

void	bngnServer::addListenSocksToEpoll() {
	for (int i = 0; i < listenSockets.size(); ++i) {
		ev.events = EPOLLIN;
		ev.data.fd = listenSockets[i];
		if (epoll_ctl(epollFd, EPOLL_CTL_ADD, listenSockets[i], &ev) == -1) {
			perror("epoll_ctl failed: "); exit(EXIT_FAILURE);
		}
	}
}

void bngnServer::createEpollInstance() {
	epollFd = epoll_create1(0);
	if (epollFd == -1) {
		perror("epoll_create1: "); exit(-1);
	}
}

void 	bngnServer::startServer() {
	int serverFd, opt = 1;

	if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed"); exit(EXIT_FAILURE);
    }
	if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt"); exit(EXIT_FAILURE);
    }
	address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
	if (bind(serverFd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed"); exit(EXIT_FAILURE);
    }
	cerr << "listening on port 8080.." << endl;
	if (listen(serverFd, 3) < 0) {
        perror("listen"); exit(EXIT_FAILURE);
    }
	listenSockets.push_back(serverFd);
}