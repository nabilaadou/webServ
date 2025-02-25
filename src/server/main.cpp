#include "server.h"

int main(int ac, char **av) {
    // configuration config;

    // config.bodySize == 100000;
    // config.errorPages[405] = "/www/static/errors/405.html";

    // config.locations["/"].uri = "/";
    // config.locations["/"].methods.push_back(GET);
    // config.locations["/"].methods.push_back(POST);
    // config.locations["/"].alias = "/www";
    // config.locations["/"].upload = "/www/upload";

    // config.locations["/html/"].uri = "/html/";
    // config.locations["/html/"].methods.push_back(GET);
    // config.locations["/html/"].alias = "/www/static/html";
    
    // config.locations["/assets/"].uri = "/assets/";
    // config.locations["/assets/"].methods.push_back(GET);
    // config.locations["/assets/"].alias = "/www/static/assets";
    // config.locations["/assets/"].index = "mov_bbb.mp4";

    // config.locations["/cgi/"].uri = "/cgi/";
    // config.locations["/cgi/"].methods.push_back(GET);
    // config.locations["/cgi/"].methods.push_back(POST);
    // config.locations["/cgi/"].alias = "/www/bin/cgi";
    // config.locations["/cgi/"].cgi[".cgi"] = "";
    // config.locations["/cgi/"].cgi[".php"] = "/usr/bin/php-cgi";
    // config.locations["/cgi/"].index = "script.cgi";

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
    catch (...) {
        cerr << "ERROR" << endl;
        return -1;
    }
    return 0;
}