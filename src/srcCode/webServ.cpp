// #include "webServ.hpp"
#include "server.hpp"
string getSupportedeExtensions(const string& key);

int main(int ac, char **av) {
    if (ac != 2) {
        cout << "invalid number of argument" << endl;
        return 1;
    }
    (void)av;
    getSupportedeExtensions("");
    bngnServer tt;
    return 0;
}