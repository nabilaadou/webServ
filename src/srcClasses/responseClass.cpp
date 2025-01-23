#include "webServ.hpp"

void webServ::handelNewConnection(int eventFd) {
    int clientFd;

    if ((clientFd = accept(eventFd, NULL, NULL)) == -1) {
        cerr << "Accept failed" << endl;
        return ;
    }
    cout << "New client connected!" << endl;

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
    cout << "BA##-> " << indexMap[clientFd].clientFd << endl;
}

// void webServ::handelClientReq(int& i) {
//     int bytesRead = ft_recv(events[i].data.fd);
//     if (bytesRead < 0) { return; } 
//     else if (bytesRead == 0 && buffer.empty()) { return; }


//     buffer[bytesRead] = 0;
//     cout << "request------>\n" << buffer << endl;

//     method = "None";
//     if (buffer[0] == 'G') {
//         method = "GET";
//     }
//     indexMap[events[i].data.fd].requestedFile = getFile(buffer);
//     buffer.clear();
// }

void webServ::handelClientRes_1(int clientFd) {
    if (indexMap[clientFd].method != "GET" || indexMap[clientFd].headerSended == true)
        return;
    struct stat file_stat;

    if (stat(indexMap[clientFd].requestedFile.c_str(), &file_stat) != 0) {
        std::cerr << "Could not open the file: " << indexMap[clientFd].requestedFile + "\n";
        statusCode = 404;
        return ;
    }
    if (!(file_stat.st_mode & S_IRUSR)) {
        std::cerr << "user don't have Permissions.\n";
        statusCode = 403;
        return ;
    }

    string response;
    cout << "HERE\n";
    if (file_stat.st_size < 10000) {
        ifstream fileStream(indexMap[clientFd].requestedFile.c_str(), std::ios::binary);
        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        string body = buffer.str();
        response = "HTTP/1.1 200 OK\r\n" + fileType +
                        "Content-Length: " + toString(body.size()) + "\r\n" +
                        "Connection: close" + string("\r\n\r\n");
        response += body;
        send(clientFd, response.c_str(), response.size(), MSG_DONTWAIT);
        ev.events = EPOLLIN ;                                         // monitor for incoming data (add `EPOLLET` for edge-triggered mode)
        ev.data.fd = clientFd;
        epoll_ctl(epollFd, EPOLL_CTL_MOD, clientFd, &ev);
        fileStream.close();
        if (clientFd >= 0) {
            ft_close(clientFd, "clientFd");
            clientFd = -1;
        }
    }
    else {
        indexMap[clientFd].headerSended = true;
        indexMap[clientFd].fileFd = open(indexMap[clientFd].requestedFile.c_str(), O_RDONLY);
        if (indexMap[clientFd].fileFd < 0)
            perror("HERE");
        response =   "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n" +
                            fileType + "Connection: close" + string("\r\n\r\n");
        send(clientFd, response.c_str(), response.size(), MSG_DONTWAIT);
    }
}

void webServ::handelClientRes_2(int clientFd) {
    if (indexMap[clientFd].method != "GET" || clientFd < 0 || indexMap[clientFd].fileFd < 0 || indexMap[clientFd].headerSended == false)
        return;
    const ssize_t chunkSize = 10000;
    char buffer[chunkSize+1];
    ssize_t bytesRead = read(indexMap[clientFd].fileFd, buffer, chunkSize);
    buffer[bytesRead] = '\0';
    if (bytesRead < 0) {
        perror("ba33");
        return ;
    }
    if (bytesRead > 0) {
        std::ostringstream chunkSizeStream;
        chunkSizeStream << std::hex << bytesRead << "\r\n";
        std::string chunkSizeStr = chunkSizeStream.str();

        send(clientFd, chunkSizeStr.c_str(), chunkSizeStr.size(), MSG_DONTWAIT);
        send(clientFd, buffer, bytesRead, MSG_DONTWAIT);
        send(clientFd, "\r\n", 2, MSG_DONTWAIT);
    }
    if (bytesRead < chunkSize) {
        send(clientFd, "0\r\n\r\n", 5, MSG_DONTWAIT);
        ev.events = EPOLLIN ;
        ev.data.fd = clientFd;
        epoll_ctl(epollFd, EPOLL_CTL_MOD, clientFd, &ev);
        if (indexMap[clientFd].fileFd >= 0) {
            ft_close(indexMap[clientFd].fileFd, "fileFd");
            indexMap[clientFd].fileFd = -1;
        }
        if (clientFd >= 0) {
            ft_close(clientFd, "clientFd");
            clientFd = -1;
        }
        return ;
    }
}



