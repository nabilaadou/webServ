#include <vector>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <algorithm>
#include<map>

#include <request.hpp>
#include <response.hpp>
#define MAX_EVENTS 10

using namespace std;

typedef struct s_ {
	Request		req;
	Response	res;
	int			statusCode;
	string		codeMeaning;
} t_;


class bngnServer {
	private:
		struct sockaddr_in	address;
		struct epoll_event	ev, events[MAX_EVENTS];
		int 				epollFd;
		vector<int>			listenSockets;
		map<int, t_>		handelers;

		void	startServer();
		void	addListenSocksToEpoll();
		void	createEpollInstance();
		void	acceptNewClient(const int);
		void	IOMultiplexer();
	public:
		bngnServer();
};