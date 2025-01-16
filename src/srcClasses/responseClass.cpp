#include "webServ.hpp"

void webServ::handelClientReq(int& i) {
    clientFd = events[i].data.fd;
    if (indexMap[clientFd].headerSended == true) { return; }
    int bytesRead = ft_recv(clientFd);
    if (bytesRead < 0) { return; } 
    else if (bytesRead == 0 && buffer.empty()) { return; }


    // print client requeset
    buffer[bytesRead] = 0;
    // cout << "request------>\n" << buffer << endl;

    if (buffer[0] == 'G') {
        method = "GET";
    }
    requestedFile = getFile(buffer);
    buffer.clear();
}



void webServ::handelClientRes_1() {
    if (method != "GET" || indexMap[clientFd].headerSended == true)
        return;

    indexMap[clientFd].requestedFile = requestedFile;
    struct stat file_stat;
    if (stat(requestedFile.c_str(), &file_stat) != 0) {
        std::cerr << "Could not open the file: " << requestedFile + "\n";
        statusCode = 404;
        return ;
    }
    if (!(file_stat.st_mode & S_IRUSR)) {
        std::cerr << "user don't have Permissions.\n";
        statusCode = 403;
        return ;
    }

    string response;
    if (file_stat.st_size < 10000) {
        ifstream fileStream(requestedFile.c_str(), std::ios::binary);
        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        string body = buffer.str();
        string response = "HTTP/1.1 200 OK\r\n" + fileType +
                        "Content-Length: " + toString(body.size()) + "\r\n" +
                        "Connection: close" + string("\r\n\r\n");
        response += body;
        send(clientFd, response.c_str(), response.size(), MSG_DONTWAIT);
        ft_close(clientFd);
    }
    else {
        indexMap[clientFd].fileFd = open(indexMap[clientFd].requestedFile.c_str(), O_RDONLY);
        indexMap[clientFd].headerSended = true;
        string response =   "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n" +
                            fileType + "Connection: close" + string("\r\n\r\n");
        send(clientFd, response.c_str(), response.size(), MSG_DONTWAIT);
    }
}

void webServ::handelClientRes_2() {
    if (method != "GET" || indexMap[clientFd].headerSended == false)
        return;
    const size_t chunkSize = 10000;
    size_t bytesRead = 1;
    char buffer[chunkSize];
    while(bytesRead > 0) {
        bytesRead = read(indexMap[clientFd].fileFd, buffer, chunkSize);
        
        indexMap[clientFd].bytes_sent += bytesRead;
        if (bytesRead > 0) {
            std::ostringstream chunkSizeStream;
            chunkSizeStream << std::hex << bytesRead << "\r\n";
            std::string chunkSizeStr = chunkSizeStream.str();

            send(clientFd, chunkSizeStr.c_str(), chunkSizeStr.size(), MSG_DONTWAIT);
            send(clientFd, buffer, bytesRead, MSG_DONTWAIT);
            send(clientFd, "\r\n", 2, MSG_DONTWAIT);
        }
        if ((size_t)bytesRead == 0) {
            send(clientFd, "0\r\n\r\n", 5, MSG_DONTWAIT);
            ft_close(indexMap[clientFd].fileFd);
            ft_close(clientFd);
        }
    }
}



