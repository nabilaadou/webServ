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
#include "requestParse.hpp"

#define MAX_EVENTS  10

struct resReq {
    Request     req;

    int         clientFd;
    int         fileFd;
    string      requestedFile;
    bool        headerSended;
    string      method;
};

typedef map<string, string> e_map;
typedef map<int, resReq>    r_map;

class webServ {
    private:
        // int                 file;      // the file passed as arguments
        std::vector<int>    serverFd;
        int                 epollFd;

        e_map               extensions;
        e_map               data;
        r_map               indexMap;

        int                 statusCode;
        string              fileType;
        string              buffer;

        struct epoll_event  ev;
        struct epoll_event  events[MAX_EVENTS];

    public:
        webServ();
        ~webServ();

        // to do
        // openFile();                          // open the files (configuration files)
        // readConfigurationFile();             // read and applay configuration file

        vector<int>     getPorts();
        vector<string>  split_string(const string& str, const string& delimiters);
        string          getBody(string str);
        e_map           getSupportedeExtensions();

        void createSockets();
        void startSocket(const int& port);
        void startEpoll();
        void reqResp();


        void handelNewConnection(int eventFd);
        void handelClientRes_1(int FD);
        void handelClientRes_2(int FD);


        // string GET(const string& requestedFile);
        // string POST(string requeste);
        // void DELETE()
};

string                          toString(const int& nbr);

#endif
