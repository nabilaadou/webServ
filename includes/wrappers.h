#pragma once
#include <cstdlib>
#include <unistd.h>
#include <statusCodeException.hpp>

using namespace std;

string	w_realpath(const char * file_name);
ssize_t w_write(int fildes, const void *buf, size_t nbyte);