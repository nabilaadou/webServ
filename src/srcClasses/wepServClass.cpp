#include "webServ.hpp"

// webServ::webServ(int fd) : fd(fd) { }

// webServ::webServ(const webServ& other) {
//     *this = other;
// }
// webServ& webServ::operator=(const webServ& other) {
//     if (this != &other) {
//         this->fd = other.fd;
//     }
//     return (*this);
// }

// webServ::~webServ() { }

vector<int> webServ::getPorts() {
    vector<int> ports;
    ports.push_back(8080);
    ports.push_back(9090);
    return (ports);
}

void webServ::createSockets() {
    vector<int> ports = getPorts();
    for (int port = 0; port < (int)ports.size(); ++port) {
        startSocket(ports[port]);
    }
}

void webServ::startSocket(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        throw "Socket creation failed";
    }
    
    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(fd);
        throw "setsockopt failed";
    }

    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
        close(fd);
        throw "setsockopt failed";
    }

    struct sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    
    if (bind(fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        close(fd);
        throw "Bind failed";
    }

    if (listen(fd, 3) < 0) {
        close(fd);
        throw "listen failed";
    }
    serverFd.push_back(fd);
}

void webServ::startEpoll() {
    epollFd = epoll_create1(0);
    if (epollFd == -1) {
        throw "epoll_create1 failed";
    }

    struct epoll_event event = {};
    for (int fd = 0; fd < (int)serverFd.size(); ++fd) {
        event.events = EPOLLIN;
        event.data.fd = serverFd[fd];
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverFd[fd], &event) < 0) {
            close(epollFd);
            throw "Epoll ctl failed";
        }
    }
}



void webServ::reqResp() {
    char buffer[BUFFER_SIZE];
    int nfds;

    cout << "Server listening on port XX..." << endl;
    while (true) {
        if ((nfds = epoll_wait(epollFd, events, MAX_EVENTS, -1)) == -1) {
            cout << "epoll_wait failed" << endl;
            break;
        }

        for (int i = 0; i < nfds; i++) {
            int eventFd = events[i].data.fd;
            if (find(serverFd.begin(), serverFd.end(), eventFd) != serverFd.end()) {
                clientFd = accept(eventFd, NULL, NULL);
                if (clientFd == -1) {
                    cout << "Accept failed" << endl;
                    continue;
                }

                cout << "New client connected!" << endl;

                ev.events = EPOLLIN;
                ev.data.fd = clientFd;
                if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev) == -1) {
                    cout << "epoll_ctl failed for client socket" << endl;
                    continue;
                }
            } else {
                clientFd = events[i].data.fd;
                int bytesRead = recv(clientFd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
                if (bytesRead < 0) {
                    cout << "Recv failed" << endl;
                    close(clientFd);
                    continue;
                } else if (bytesRead == 0) {
                    cout << "Client disconnected!" << endl;
                    close(clientFd);
                    continue;
                }
                buffer[bytesRead] = 0;
                cout << "Received request: " << buffer << endl;
                const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nHello, world!";
                send(clientFd, response, strlen(response), MSG_DONTWAIT);

                cout << "Response sent to client!" << endl;
                close(clientFd);
            }
        }
    }
    for (int fd = 0; fd < (int)serverFd.size(); ++fd) {
        close(serverFd[fd]);
    }
    close(epollFd);
}