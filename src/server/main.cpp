#include "server.h"
using namespace std;

int main(int ac, char **av) {

    configuration config;
    config.bodySize == 100000;
    config.loctions["/html/"].methods.push_back("GET");
    config.loctions["/html/"].root = "www/static";
    config.loctions["/html/"].index = "index.html";



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