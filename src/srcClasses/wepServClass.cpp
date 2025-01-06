#include "webServ.hpp"

webServ::webServ(int fd) : fd(fd) { }

webServ::webServ(const webServ& other) {
    *this = other;
}
webServ& webServ::operator=(const webServ& other) {
    if (this != &other) {
        this->fd = other.fd;
    }
    return (*this);
}

webServ::~webServ() { }
