#ifndef webServ_HPP
#define webServ_HPP

using namespace std;

#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include "wrapperFunc.hpp"


#define BUFFER_SIZE 1024
#define MAX_EVENTS 10

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
