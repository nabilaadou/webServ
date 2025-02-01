#include "Cgi.hpp"
#include <iostream>
#include <string.h>
#include <unordered_map>
#include <vector>
#include <stack>
#include <sstream>
#include <algorithm>
#include <unistd.h>
#include <arpa/inet.h>//for frecv
#include <sys/stat.h>
#include "statusCodeException.hpp"

using namespace std;
class Response {
	private:
		int		clientFd;

		void	sendStartLine();
		void	sendHeaders();
		void	sendBody();
	public:
		Response();

};