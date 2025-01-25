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
    char        resolvedPath[PATH_MAX];
    struct stat file_stat;
    string      reason;


    statusCode = 200;
    if (realpath(indexMap[clientFd].requestedFile.c_str(), resolvedPath)) {
        string file = resolvedPath;
        if (file.find(DOCUMENT_ROOT) == 0) {
            std::cerr << "Directory Traversal Attempt.\n";
            statusCode = 403;
            reason = " 403 Forbidden";
        }
    }
    if (statusCode == 200 && stat(indexMap[clientFd].requestedFile.c_str(), &file_stat) == -1) {
        if (errno == ENOENT) {
            std::cerr << "No such file or directory: " << indexMap[clientFd].requestedFile + "\n";
            statusCode = 404;
            reason = " 404 Not Found";
        }
        else {
            std::cerr << "stat() failed\n";
            statusCode = 500;
            reason = " 500 Internal Server Error";
        }
    }
    else if (statusCode == 200 && S_ISDIR(file_stat.st_mode) != 0) {
        indexMap[clientFd].requestedFile = indexMap[clientFd].requestedFile + "/index.html";
        fileType = extensions[".html"];
        handelClientRes(clientFd);
        return ;
    }
    else if (statusCode == 200 && S_ISREG(file_stat.st_mode) != 0) {
        if (!(file_stat.st_mode & S_IRUSR)) {
            std::cerr << "user don't have Permissions.\n";
            statusCode = 403;
            reason = " 403 Forbidden";
        }
    }






    
    if (statusCode < 300) {
        if (file_stat.st_size < 10000)
            sendRes(clientFd, true);
        else
            sendRes(clientFd, false);
    }
    else {                       // send ERROR response
        string response = "HTTP/1.1 " + reason + string("\r\n") + "content-length: " + toString(31+reason.size()) + "\r\n" + extensions[".html"] + "\r\n\r\n" + "<header><h1>ERROR"+reason+"</h1></header>\r\n";
        send(clientFd, response.c_str(), response.size(), MSG_DONTWAIT);
        ev.events = EPOLLIN ;
        ev.data.fd = clientFd;
        epoll_ctl(epollFd, EPOLL_CTL_MOD, clientFd, &ev);
    }
}

void webServ::sendRes(int clientFd, bool smallFile) {
    if (indexMap[clientFd].method == "GET") {
        if (indexMap[clientFd].headerSended == false) {
            GET(clientFd, smallFile);
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




