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
    memset(&serverAddress, 0, sizeof(serverAddress));
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

    std::cout << "Server listening on port XX..." << std::endl;
    clientFd = accept(serverFd, (struct sockaddr*)&serverAddress, &clientAddresslen);
    if (clientFd < 0) {
        close(serverFd);
        throw "Accept failed";
    }

    char buffer[1024] = {0};
    int bytes_read = read(clientFd, buffer, sizeof(buffer));
    if (bytes_read < 0) {
        close(serverFd);
        throw "Accept failed";
    }
    else {
        std::cout << "\nReceived request--->\n" << buffer << "\n\n\n";
    }

    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\nHello, world!";
    send(clientFd, response.c_str(), response.size(), 0);
    std::cout << "Response sent----->\n" << response << "\n";

    close(clientFd);
    close(serverFd);
}