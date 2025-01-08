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

void webServ::startSocket(int port) {
    int fd = ft_socket(AF_INET, SOCK_STREAM, 0); // create a socket

    ft_setsockopt(fd, SOL_SOCKET, SO_REUSEADDR); // configure socket options, see man setsockopt

    ft_fcntl(fd, F_SETFL, O_NONBLOCK);

    struct sockaddr_in address = {};
    address.sin_family = AF_INET;          // AF_INET = ipv4
    address.sin_addr.s_addr = INADDR_ANY;  // INADDR_ANY = listen on all network interfaces
    address.sin_port = htons(port);        // convert port to network byte order
    
    ft_bind(fd, (struct sockaddr*)&address, sizeof(address)); // bind socket to its corresponding port.
    ft_listen(fd, 3);  // the server waits for someone to connect (like a browser).
    serverFd.push_back(fd); // add that socket the serverFd
}

// initialize 1 socket for each port
void webServ::createSockets() {
    vector<int> ports = getPorts();
    for (int port = 0; port < (int)ports.size(); ++port) {
        startSocket(ports[port]);
    }
}

// startEpoll and add server sockets to the epoll instance
void webServ::startEpoll() {
    epollFd = ft_epoll_create1(0); // Create an epoll instance

    struct epoll_event event = {};
    for (int fd = 0; fd < (int)serverFd.size(); ++fd) {
        event.events = EPOLLIN;       // monitor for incoming connections
        event.data.fd = serverFd[fd]; // associate the file descriptor
        ft_epoll_ctl(epollFd, EPOLL_CTL_ADD, serverFd[fd], &event);
    }
}

void webServ::reqResp() {
    char buffer[BUFFER_SIZE];
    int nfds;

    cout << "Server listening on port XX..." << endl;
    while (true) {
        // wait for events on the monitored sockets
        nfds = ft_epoll_wait(epollFd, events, MAX_EVENTS, -1, serverFd, epollFd);

        for (int i = 0; i < nfds; i++) {
            int eventFd = events[i].data.fd;

            // check if this is a server socket = new connection
            if (find(serverFd.begin(), serverFd.end(), eventFd) != serverFd.end()) {
                if ((clientFd = accept(eventFd, NULL, NULL)) == -1) {
                    cerr << "Accept failed" << endl;
                    continue;
                }
                cout << "New client connected!" << endl;

                // set the client socket to non-blocking mode
                if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0) {
                    cerr << "Failed to set non-blocking" << endl;
                    ft_close(clientFd);
                    continue;
                }


                // add the new client socket to epoll
                ev.events = EPOLLIN;                        // monitor for incoming data (add `EPOLLET` for edge-triggered mode)
                ev.data.fd = clientFd;
                if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev) == -1) {
                    cerr << "epoll_ctl failed for client socket" << clientFd << endl;
                    ft_close(clientFd);
                    continue;
                }
            }              // client socket activity
            else {
                clientFd = events[i].data.fd;
                int bytesRead = recv(clientFd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
                if (bytesRead < 0) {
                    cerr << "Recv failed" << endl;
                    ft_close(clientFd);
                    continue;
                } else if (bytesRead == 0) {
                    cout << "Client disconnected!" << endl;
                    ft_close(clientFd);
                    continue;
                }
                // print client requeset
                // buffer[20] = 0;
                // cout << "Received request: " << buffer << endl;
                const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nHello, world!";
                send(clientFd, response, strlen(response), MSG_DONTWAIT);
                cout << "Response sent to client!" << endl;
                ft_close(clientFd);
            }
        }
    }
}
