#include "server.h"

int main(int ac, char **av) {
    signal(SIGPIPE, SIG_IGN);
    if (ac != 2) {
        cerr << "invalid number of argument" << endl;
        return -1;
    }
    vector<int>                 servrSocks;
    int                         epollFd;
    map<string, configuration> config;
    try {
        ConfigFileParser confi(av[1]);
        config = confi.parseFile();
        // confi.printprint();
        epollFd = createSockets(config, servrSocks);
        while (1) { //this loop is here if epoll fd somehow got closed and epoll wait fails and i have to create and instance of epoll fd;
            multiplexerSytm(servrSocks, epollFd, config);
            epollFd = startEpoll(servrSocks);
        }
    }
    catch (const exception& msg) {
        cerr << msg.what() << endl;
        return -1;
    }
    return 0;
}