#include "webServ.hpp"

void webServ::handelClient(int& i) {
    clientFd = events[i].data.fd;
    int bytesRead = ft_recv(clientFd);
    if (bytesRead < 0) { return; } 
    else if (bytesRead == 0 && buffer.empty()) { return; }


    // print client requeset
    buffer[bytesRead] = 0;
    // if (buffer[0] != 'G')
    // cout << "------->Received request:\n" << buffer << "\n\n";

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
