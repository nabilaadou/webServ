#include "webServ.hpp"

void webServ::handelNewConnection(int eventFd) {
    int clientFd;

    if ((clientFd = accept(eventFd, NULL, NULL)) == -1) {
        cerr << "Accept failed" << endl;
        return ;
    }
    cout << "\nNew client connected!\n" << endl;

    // set the client socket to non-blocking mode
    if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0) {
        cerr << "Failed to set non-blocking" << endl;
        ft_close(clientFd, "clientFd");
        return ;
    }

    // add the new client socket to epoll
    ev.events = EPOLLIN ;                                         // monitor for incoming data (add `EPOLLET` for edge-triggered mode)
    ev.data.fd = clientFd;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev) == -1) {
        cerr << "epoll_ctl failed for client socket" << clientFd << endl;
        ft_close(clientFd, "clientFd");
        return ;
    }
    indexMap[clientFd].headerSended = false;
    indexMap[clientFd].clientFd = clientFd;
    indexMap[clientFd].fileFd = -1;
}

void webServ::handelClientRes(int clientFd) {
    if (indexMap[clientFd].method == "GET") {
        if (indexMap[clientFd].headerSended == false) {
            GET(clientFd);
        }
        else {
            sendBodyifChunked(clientFd);
        }
    }
    if (indexMap[clientFd].method == "POST") {
        cout << "POST method called\n";
    }
    if (indexMap[clientFd].method == "DELETE") {
        cout << "DELETE method called\n";
    }
}





