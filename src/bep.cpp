#include "server.h"

int main() {
	char buff[1024] = {0};
    string t = "hello";
	stringstream ss(t);
	ss.read(buff, 10);
	ss.read(buff, 10);
	ss.read(buff, 10);
	ss.read(buff, 10);
	ss.read(buff, 10);
	ss.read(buff, 10);
	ss.read(buff, 10);
	ss.read(buff, 10);
	cerr << buff << endl;
}



// POST /upload HTTP/1.1
// Host: localhost:8080
// Content-Type: multipart/form-data; boundary=----WebKitFormBoundary12345
// content-length: 100
// chunked

// ------WebKitFormBoundary12345
// Content-Disposition: form-data; name="file1"; filename="image1.jpg"
// Content-Type: image/jpeg

// (binary content for image1.jpg)

// ------WebKitFormBoundary12345
// Content-Disposition: form-data; name="file2"; filename="image2.jpg"
// Content-Type: image/jpeg

// (binary content for image2.jpg)

// ------WebKitFormBoundary12345--