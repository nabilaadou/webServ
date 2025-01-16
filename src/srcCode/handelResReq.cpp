#include "webServ.hpp"

void webServ::handelNewConnection(int eventFd) {
    if ((clientFd = accept(eventFd, NULL, NULL)) == -1) {
        cerr << "Accept failed" << endl;
        return ;
    }
    cout << "New client connected!" << endl;

    // set the client socket to non-blocking mode
    if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0) {
        cerr << "Failed to set non-blocking" << endl;
        ft_close(clientFd);
        return ;
    }

    // add the new client socket to epoll
    ev.events = EPOLLIN | EPOLLET | EPOLLOUT;                        // monitor for incoming data (add `EPOLLET` for edge-triggered mode)
    ev.data.fd = clientFd;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev) == -1) {
        cerr << "epoll_ctl failed for client socket" << clientFd << endl;
        ft_close(clientFd);
        return ;
    }
    indexMap[clientFd].headerSended = false;
    indexMap[clientFd].clientFd = clientFd;
    indexMap[clientFd].bytes_sent = 0;
}

void webServ::handelClient(int& i) {
    clientFd = events[i].data.fd;
    int bytesRead = ft_recv(clientFd);
    if (bytesRead < 0) { return; } 
    else if (bytesRead == 0 && buffer.empty()) { return; }


    // print client requeset
    buffer[bytesRead] = 0;
    // if (buffer[0] != 'G')
    cout << "------->Received request:\n" << buffer << "\n\n";

    // prsing request


    string body;
    if (buffer[0] == 'G') {
        body = GET(getFile(buffer));
        buffer.clear();
    }
    // else if (buffer[0] == 'p')
    //     body = POST(getBody(buffer));
    ft_close(clientFd);
}
