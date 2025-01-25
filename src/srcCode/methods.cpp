#include "webServ.hpp"

void webServ::GET(int clientFd, bool smallFile) {
    string response;

    if (smallFile) {
        ifstream fileStream(indexMap[clientFd].requestedFile.c_str(), std::ios::binary);
        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        string body = buffer.str();
        response = "HTTP/1.1 200 OK\r\n" + fileType +
                        "Content-Length: " + toString(body.size()) + "\r\n" +
                        "Connection: close" + string("\r\n\r\n");
        response += body;
        send(clientFd, response.c_str(), response.size(), MSG_DONTWAIT);
        ev.events = EPOLLIN ;                                               // monitor for incoming data (add `EPOLLET` for edge-triggered mode)
        ev.data.fd = clientFd;
        epoll_ctl(epollFd, EPOLL_CTL_MOD, clientFd, &ev);
        fileStream.close();
        if (clientFd >= 0)
            ft_close(clientFd, "clientFd");
        cout << "done sending the response" << endl;
    }
    else {
        indexMap[clientFd].headerSended = true;
        indexMap[clientFd].fileFd = open(indexMap[clientFd].requestedFile.c_str(), O_RDONLY);
        response =   "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n" +
                            fileType + "Connection: close" + string("\r\n\r\n");
        send(clientFd, response.c_str(), response.size(), MSG_DONTWAIT);
    }
}

void webServ::sendBodyifChunked(int clientFd) {
    char buffer[BUFFER_SIZE+1];
    ssize_t bytesRead = read(indexMap[clientFd].fileFd, buffer, BUFFER_SIZE);

    buffer[bytesRead] = '\0';
    if (bytesRead < 0) {
        perror("ba33");
    }
    else if (bytesRead > 0) {
        std::ostringstream BUFFER_SIZEStream;
        BUFFER_SIZEStream << std::hex << bytesRead << "\r\n";
        std::string BUFFER_SIZEStr = BUFFER_SIZEStream.str();
        send(clientFd, BUFFER_SIZEStr.c_str(), BUFFER_SIZEStr.size(), MSG_DONTWAIT);
        send(clientFd, buffer, bytesRead, MSG_DONTWAIT);
        send(clientFd, "\r\n", 2, MSG_DONTWAIT);
    }
    else if (bytesRead < BUFFER_SIZE) {
        send(clientFd, "0\r\n\r\n", 5, MSG_DONTWAIT);
        ev.events = EPOLLIN ;
        ev.data.fd = clientFd;
        epoll_ctl(epollFd, EPOLL_CTL_MOD, clientFd, &ev);
        if (indexMap[clientFd].fileFd >= 0)
            ft_close(indexMap[clientFd].fileFd, "fileFd");
        if (clientFd >= 0)
            ft_close(clientFd, "clientFd");
        cout << "done sending the response" << endl;
    }
}
