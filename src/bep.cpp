#include "server.h"
#include "binarystring.hpp"

// c++ -fsanitize=address src/bstring/*.cpp  src/bep.cpp -I includes && ./a.out

int main() {
	bstring s("-1-2", 4);
	bstring s1;
	s1 += s;

	cerr << s1 << endl;
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