#pragma once
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <stdlib.h>
#include <iostream>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "httpSession.hpp"
#include "confiClass.hpp"
#include <statusCodeException.hpp>

#define MAX_EVENTS 10

using namespace std;

typedef struct sockaddr_in t_sockaddr;
typedef map<int, t_sockaddr>::const_iterator t_sockaddr_it;

int		createSockets(map<string, configuration>& config, vector<int>& serverFds);
int		startEpoll(const vector<int>& serverFds);
int		errorResponse(const int epollFd, const statusCodeException& exception, httpSession* session);
void	multiplexerSytm(const vector<int>& serverFds, const int epollFd, map<string, configuration>& config);
