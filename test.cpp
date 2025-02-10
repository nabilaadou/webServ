#include <string>
#include <iostream>

using namespace std;

void	mapUri(const string& uri) {
	size_t	pos = 0;
	while (1) {
		pos = uri.find('/', pos);
		string subUri = uri.substr(0, pos);
		cerr << subUri << endl;
		if (subUri == "/var/www")
			cerr << "new path: " << "/alias" + uri.substr(pos) << endl;
		if (pos++ == string::npos)
			break;
	}
}

int main() {
	string uri = "/var/www/html/index.html/";
	mapUri(uri);
}