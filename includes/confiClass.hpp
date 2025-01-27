#ifndef CONFICLASS_HPP
#define CONFICLASS_HPP

using namespace std;

# include <iostream>
# include <sstream>
# include <fstream>
# include <cstring>
# include <vector>
# include <map>
# include <algorithm>
# include <unistd.h>
# include <limits.h>
# include <cerrno>
# include <arpa/inet.h>

struct keyValue {
    int             port;
    string          host;
    vector<string>  serNames;
    vector<string>  locs;
};


class confiClass {
    private:
        string file;
        map<int, keyValue> kValue;

    public:
        confiClass(string _file);
        ~confiClass();

        void parseFile();

        void handlePort(string& line, int len);
        void handlehost(string& line, int len);
        void handleSerNames(string& line, int len);
        void handlelocs(string& line, int len);

        void printKeyValue();
};





#endif
