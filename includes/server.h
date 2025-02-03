#pragma once
#include <map>
#include <vector>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <request.hpp>
#include <response.hpp>
#include <statusCodeException.hpp>

#define MAX_EVENTS 10

using namespace std;
typedef struct sockaddr_in t_sockaddr;
typedef map<int, t_sockaddr>::const_iterator t_sockaddr_it;

typedef struct s_httpSession {
	Request		req;
	Response	res;
} t_httpSession;

void	startServer();
void	multiplexerSytm(map<int, t_sockaddr>& servrSocks, const int& epollFd);
