#include "server.h"

void	split(const string& str, const char delimiter, vector<string>& parts) {
	int	i = 0, pos = 0;

	while (str[i]) {
		if (str[i] == delimiter) {
			parts.push_back(str.substr(pos, i - pos));
			while (str[i] && str[i] == delimiter)
				++i;
			pos = i;
		} else
			++i;
	}
	parts.push_back(str.substr(pos));
}

inline std::string& trim(std::string& s) {
	s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
	s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
    return s;
}

static string	retrieveFilename(const string& value) {
	vector<string>	fieldValueparts;
	split(value, ';', fieldValueparts);
	for (const auto& it : fieldValueparts)
		cerr << it << endl;
	if (fieldValueparts.size() != 3)
		return "UNVALID1";
	if (strncmp("form-data" ,trim(fieldValueparts[0]).c_str(), 9))
		return "UNVALID2";
	vector<string> keyvalue;
	split(trim(fieldValueparts[1]), '=', keyvalue);
	if (keyvalue.size() != 2 || strncmp("name", keyvalue[0].c_str(), 4) || keyvalue[1][0] != '"' || keyvalue[1][keyvalue[1].size()-1] != '"')
		return "UNVALID3";
	keyvalue.clear();
	split(trim(fieldValueparts[2]), '=', keyvalue);
	if (keyvalue.size() != 2 || strncmp("filename", keyvalue[0].c_str(), 8) || keyvalue[1][0] != '"' || keyvalue[1][keyvalue[1].size()-1] != '"')
		return "UNVALID4";
	keyvalue[1].erase(keyvalue[1].begin());
	keyvalue[1].erase(keyvalue[1].end()-1);
	return keyvalue[1];
}

int main() {
	string line;
	string buffer = " form-data; name=\"file1\";";
	cerr << retrieveFilename(buffer) << endl;
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