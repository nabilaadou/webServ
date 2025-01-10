#include "webServ.hpp"

void webServ::handelClient(int& i) {
    char buffer[BUFFER_SIZE];
    clientFd = events[i].data.fd;
    int bytesRead = recv(clientFd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
    if (bytesRead < 0) {
        cerr << "Recv failed" << endl;
        ft_close(clientFd);
        return ;
    } else if (bytesRead == 0) {
        cout << "Client disconnected!" << endl;
        ft_close(clientFd);
        return ;
    }

    // print client requeset
    buffer[bytesRead] = 0;
    // cout << "Received request: " << buffer << endl;
    
    string body = GET(getFile(buffer));
    string response = "HTTP/1.1 " + toString(statusCode) +
                        " OK\r\n" + fileType + "Content-Length: " +
                        toString(body.size()) + string("\r\n\r\n") + body;

    send(clientFd, response.c_str(), strlen(response.c_str()), MSG_DONTWAIT);
    // cout << "Response sent to client:\n" << response << endl;
    ft_close(clientFd);
}
