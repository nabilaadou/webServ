#ifndef webServ_HPP
#define webServ_HPP

using namespace std;

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <vector>
#include <map>
#include <algorithm>
#include "wrapperFunc.hpp"

#define BUFFER_SIZE 1024
#define MAX_EVENTS 10

typedef map<string, string> u_map;

class webServ {
    private:
        int                 file;      // the file passed as arguments
        std::vector<int>    serverFd;
        int                 clientFd;
        int                 epollFd;

        u_map               extensions;

        int                 statusCode;
        string              fileType;

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
        string      getFile(const string str);
        u_map       getSupportedeExtensions();

        void createSockets();
        void startSocket(const int& port);
        void startEpoll();
        void reqResp();

        void handelClient(int& i);


        string GET(const string& requestedFile);
        // void POST()
        // void DELETE()
};

string                          toString(const int& nbr);




#endif
