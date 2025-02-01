#include <vector>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <algorithm>

#include <requestParse.hpp>

using namespace std;

#define MAX_EVENTS 10

class bngnServer {
	private:
		//server
		vector<int>			listenSockets;
		struct sockaddr_in	address;
		//epoll
		int 				epollFd;
		struct epoll_event	ev, events[MAX_EVENTS];

		Request				req;

		

		void	startServer();
		void	addListenSocksToEpoll();
		void	createEpollInstance();

		void	acceptNewClient(const int);
		void	treatRequest(const int);
		void	IOMultiplexer();
	public:
		bngnServer();
};