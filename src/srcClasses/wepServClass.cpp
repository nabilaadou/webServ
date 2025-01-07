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


void webServ::startSocket() {
    char buffer[BUFFER_SIZE];
    memset(&serverAddress, 0, sizeof(serverAddress));
    memset(&clientAddress, 0, sizeof(clientAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    clientAddresslen = sizeof(clientAddress);

    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1) {
        throw "Socket creation failed";
    }

    if (bind(serverFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        close(serverFd);
        throw "Bind failed";
    }

    if (listen(serverFd, 3) < 0) {
        close(serverFd);
        throw "listen failed";
    }

    epollFd = epoll_create1(0);
    if (epollFd == -1) {
        throw "epoll_create1 failed";
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = serverFd;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverFd, &ev) == -1) {
        throw "epoll_ctl failed";
    }
    struct epoll_event events[MAX_EVENTS];

    std::cout << "Server listening on port XX..." << std::endl;
    while (true) {
        int nfds = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            cout << "epoll_wait failed" << endl;
            break;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == serverFd) {
                // sleep(2);
                clientFd = accept(serverFd, (struct sockaddr*)&clientAddress, &clientAddresslen);
                if (clientFd == -1) {
                    cout << "Accept failed" << endl;
                    continue;
                }

                std::cout << "New client connected!" << std::endl;

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
                    std::cout << "Client disconnected!" << std::endl;
                    close(clientFd);
                    continue;
                }

                std::cout << "Received request: " << buffer << std::endl;
                const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nHello, world!";
                send(clientFd, response, strlen(response), MSG_DONTWAIT);

                std::cout << "Response sent to client!" << std::endl;
                close(clientFd);
            }
        }
    }
    close(clientFd);
    close(epollFd);
    close(serverFd);
}