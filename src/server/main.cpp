#include "server.h"
using namespace std;

int main(int ac, char **av) {
    if (ac != 2) {
        cerr << "invalid number of argument" << endl;
    } else {
    	startServer();
	}
    return 0;
}