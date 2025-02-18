#include "wrappers.h"

string	w_realpath(const char * file_name) {
	char absolutePath[1024];

	if (realpath(file_name, absolutePath) == nullptr)
		throw(statusCodeException(404, "Not Found"));
	return string(absolutePath);
}

ssize_t w_write(int fildes, const void *buf, size_t nbyte) {
	int bytewrite;
	if ((bytewrite = write(fildes, buf,nbyte)) <= 0) {
		perror("write failed");
		//close client connection gracefully
	}
	return bytewrite;
}