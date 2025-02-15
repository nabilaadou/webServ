#include "server.h"

bool	getlineFromString(string& buffer, string& line) {
	line  = "";
	int i = 0;
	while (buffer[i] && buffer[i] != '\n') {
		line += buffer[i];
		++i;
	}
	if (buffer[i] == 0) {
		buffer.erase(buffer.begin(), buffer.begin()+i);
		return true;
	}
	buffer.erase(buffer.begin(), buffer.begin()+i+1);
	return false;
}

int main() {
    std::string data = "Hello, world!";  // No newline at the end
    std::stringstream ss(data);

    std::string line;
    std::getline(ss, line);

    std::cout << "Line: \"" << line << "\"\n";

    if (ss.eof()) {
        std::cout << "EOF reached.\n";
    }
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