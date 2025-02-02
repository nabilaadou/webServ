#include <unistd.h>
#include "statusCodeException.hpp"

#include <iostream>

ssize_t w_write(int fildes, const void *buf, size_t nbyte) {
	int byteWrite;
	if (byteWrite = write(fildes, buf, nbyte) < 0) {
		perror("write failed: ");
		throw(statusCodeException(500, "Internal Server Error"));
	}
	return byteWrite;
}