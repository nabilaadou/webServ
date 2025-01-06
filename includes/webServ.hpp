#ifndef webServ_HPP
#define webServ_HPP

#include <iostream>

using namespace std;

class webServ {
    private:
    int fd;      // the file passed as arguments
    public:
        webServ(int fd);
        webServ(const webServ& other);
        webServ& operator=(const webServ& other);
        ~webServ();

        // to do
        // openFile();                          // open the files (configuration files)
        // readConfigurationFile();             // read and applay configuration file
};

#endif
