#include <sys/epoll.h>
#include <stdlib.h>
#include <iostream>
#define MAX_EVENTS 10
void	IOMultiplexer() {
	int epollFd;
	struct epoll_event ev, events[MAX_EVENTS];
	epollFd = epoll_create1(0);
	if (epollFd == -1) {
		perror("epoll_create1: "); exit(-1);
	}
	while (1) {
		int nfds;
		if (nfds = epoll_wait(epollFd, events, MAX_EVENTS, 0) == -1) {
			perror("epoll_wait failed: "); exit(-1);
		}
		for (int i = 0; i < nfds; ++i) {
			if (find(listenSockets.begin(), listenSockets.end(), events[i].data.fd) != listenSockets.end()) {
				//handl new clients;
				acceptNewClient();
			}
			else {
				//handle request;
			}
			ev.events = EPOLLIN;
			epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev);
			
		}
	}
}
