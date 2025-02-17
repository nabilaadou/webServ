#include "server.h"
#include "binarystring.hpp"

// c++ -fsanitize=address src/bstring/*.cpp  src/bep.cpp -I includes && ./a.out

int main() {
	bstring s("1$$2$$$3", 8);

	// cerr << ((s.ncmp("$$", 2, 1) == 0) ? "equal" : "not equal") << endl;
	vector<bstring> list = s.split("$$");
	for (const auto& it : list)
		cerr << it << endl;
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