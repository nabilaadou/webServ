#include "httpSession.hpp"

httpSession::httpSession(): req(Request(*this)), res(Response(*this)), cgi(NULL) {}
