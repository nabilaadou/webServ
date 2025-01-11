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
    ev.events = EPOLLIN;                        // monitor for incoming data (add `EPOLLET` for edge-triggered mode)
    ev.data.fd = clientFd;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev) == -1) {
        cerr << "epoll_ctl failed for client socket" << clientFd << endl;
        ft_close(clientFd);
        return ;
    }
}

ssize_t webServ::ft_recv(int __fd, void *__buf) {
    char buff[BUFFER_SIZE];
    ssize_t bytesRead;

    while ((bytesRead = recv(__fd, buff, BUFFER_SIZE, MSG_DONTWAIT)) > 0) {
        buffer += string(buff);
    }

    return (bytesRead);
}

void webServ::handelClient(int& i) {
    char buffer[BUFFER_SIZE];
    clientFd = events[i].data.fd;
    int bytesRead = ft_recv(clientFd, buffer);
    if (bytesRead < 0) {
        cerr << "Recv failed" << "\n\n";
        ft_close(clientFd);
        return ;
    } else if (bytesRead == 0) {
        cout << "Client disconnected!" << "\n\n";
        ft_close(clientFd);
        return ;
    }
    // print client requeset
    buffer[bytesRead] = 0;
    if (buffer[0] != 'G')
        cout << "------->Received request:\n" << buffer << "\n\n";

    // prsing request


    string body;
    // if (buffer[0] == 'G')
    //     body = GET(getFile(buffer));
    // else if (buffer[0] == 'p')
    //     body = POST(getBody(buffer));
    
    if (buffer[0] == 'P')
        POST(getBody(buffer));
    body = GET(getFile(buffer));
    string response = "HTTP/1.1 " + toString(statusCode) +
                        " OK\r\n" + fileType + "Content-Length: " +
                        toString(body.size()) + string("\r\n\r\n") + body;

    send(clientFd, response.c_str(), response.size(), MSG_DONTWAIT);
    // cout << "----------->Response sent to client:\n" << response << endl;
    ft_close(clientFd);
}

string webServ::GET(const string& requestedFile) {
    std::fstream fileStream(requestedFile.c_str());
    if (!fileStream.is_open()) {
        std::cerr << "Could not open the file: " << requestedFile + "\n";
        statusCode = 404;
        return "";
    }

    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    statusCode = 200;
    return buffer.str();
}

string webServ::POST(const string& requestedFile) {

    return requestedFile;
}