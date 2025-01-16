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
#include <sys/stat.h>
#include <unistd.h>
#include "wrapperFunc.hpp"

#define BUFFER_SIZE 1024
#define MAX_EVENTS  10

struct resReq {
    // res     r;
    int         clientFd;
    int         fileFd;
    size_t      bytes_sent;
    string      requestedFile;
    bool        headerSended;

    // resReq(string requestedFile) : fileStream(requestedFile.c_str(), std::ios::binary) {}
};

typedef map<string, string> e_map;
typedef map<int, resReq>    r_map;

class res {
    private:
    public:
};

class webServ {
    private:
        // int                 file;      // the file passed as arguments
        std::vector<int>    serverFd;
        int                 clientFd;
        int                 epollFd;

        e_map               extensions;
        e_map               data;
        r_map               indexMap;

        int                 statusCode;
        string              fileType;
        string              requestedFile;

        string              buffer;
        string              method;


        struct epoll_event  ev;
        struct epoll_event  events[MAX_EVENTS];

    public:
        // webServ(int fd);
        // webServ(const webServ& other);
        // webServ& operator=(const webServ& other);
        // ~webServ();

        // to do
        
        // openFile();                          // open the files (configuration files)
        // readConfigurationFile();             // read and applay configuration file

        vector<int>     getPorts();
        vector<string>  split_string(const string& str, const string& delimiters);
        string          getFile(string str);
        string          getBody(string str);
        e_map           getSupportedeExtensions();
        ssize_t         ft_recv(int __fd);

        void createSockets();
        void startSocket(const int& port);
        void startEpoll();
        void reqResp();


        void handelClientReq(int& i);
        void handelClientRes_1();
        void handelClientRes_2();
        void handelClientRes_3();

        void handelClient(int& i);
        void handelNewConnection(int eventFd);


        string GET(const string& requestedFile);
        string POST(string requeste);
        // void DELETE()
};

string                          toString(const int& nbr);




#endif
