#ifndef webServ_HPP
#define webServ_HPP

#include <iostream>
#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>  // For sockaddr_in, htons, etc.
#include <arpa/inet.h>   // For inet_pton, inet_addr, etc.
#include <sys/socket.h>  // For socket, AF_INET, etc.
#include <unistd.h>      // For close()
#include <cstring>


using namespace std;

class webServ {
    private:
        int         fd;      // the file passed as arguments
        int         serverFd;
        int         clientFd;
        sockaddr_in serverAddress;
        sockaddr_in clientAddress;
        socklen_t   clientAddresslen;
    public:
        // webServ(int fd);
        // webServ(const webServ& other);
        // webServ& operator=(const webServ& other);
        // ~webServ();

        // to do
        
        // openFile();                          // open the files (configuration files)
        // readConfigurationFile();             // read and applay configuration file

        void startSocket();

        // void GET()
        // void POST()
        // void DELETE()
};

#endif
