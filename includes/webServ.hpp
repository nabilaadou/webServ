#ifndef webServ_HPP
#define webServ_HPP

#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>  // For sockaddr_in, htons, etc.
#include <arpa/inet.h>   // For inet_pton, inet_addr, etc.
#include <sys/socket.h>  // For socket, AF_INET, etc.
#include <unistd.h>      // For close()
#include <fcntl.h>
#include <cstring>
#include <vector>
#include <algorithm>

#define BUFFER_SIZE 1024
#define MAX_EVENTS 10
using namespace std;

class webServ {
    private:
        int                 file;      // the file passed as arguments
        std::vector<int>    serverFd;
        int                 clientFd;
        int                 epollFd;

        struct epoll_event ev;
        struct epoll_event events[MAX_EVENTS];
    public:
        // webServ(int fd);
        // webServ(const webServ& other);
        // webServ& operator=(const webServ& other);
        // ~webServ();

        // to do
        
        // openFile();                          // open the files (configuration files)
        // readConfigurationFile();             // read and applay configuration file

        vector<int> getPorts();

        void createSockets();
        void startSocket(int port);
        void startEpoll();
        void reqResp();

        // void GET()
        // void POST()
        // void DELETE()
};

#endif
