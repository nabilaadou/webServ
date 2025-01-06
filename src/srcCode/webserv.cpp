#include "webserv.hpp"

int main(int ac, char **av) {
    if (ac != 2) {
        cout << "invalid number of argument" << endl;
        return 1;
    }
    
    cout << av[1] << endl;

    return 0;
}