#include "server.h"
using namespace std;

int main(int ac, char **av) {
    if (ac != 2) {
        cerr << "invalid number of argument" << endl;
    } else {
        map<int, t_sockaddr>    servrSocks;
        int                     epollFd;
    	epollFd = startServer(servrSocks);
        multiplexerSytm(servrSocks, epollFd);
	}
    return 0;
}