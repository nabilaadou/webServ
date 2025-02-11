#include "server.h"

int main(int ac, char **av) {

    configuration config;
    config.bodySize == 100000;
    config.errorPages[405] = "/www/static/errors/405.html";
    config.loctions["/html/"].uri = "/html/";
    config.loctions["/html/"].methods.push_back("GET");
    config.loctions["/html/"].alias = "/www/static/html";
    
    
    config.loctions["/assets/"].uri = "/assets/";
    config.loctions["/assets/"].methods.push_back("GET");
    config.loctions["/assets/"].alias = "/www/static/assets";



    if (ac != 2) {
        cerr << "invalid number of argument" << endl;
    } else {
        map<int, t_sockaddr>    servrSocks;
        int                     epollFd;
    	epollFd = startServer(servrSocks);
        multiplexerSytm(servrSocks, epollFd, config);
	}
    return 0;
}