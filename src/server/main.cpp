#include "server.h"

int main(int ac, char **av) {
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
        multiplexerSytm(servrSocks, epollFd, config);
    }
    catch (const exception& msg) {
        cerr << msg.what() << endl;
        return -1;
    }
    return 0;
}