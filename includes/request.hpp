#pragma once

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

#define BUFFER_SIZE 8192 

using namespace std;

string  trim(const string& str);

class Cgi;

typedef enum e_requestState{
	PROCESSING,
	DONE,
	CCLOSEDCON,
}	t_requestState;

//echo -e "GET / HTTP/1.1\r\n    Host: localhost\r\n\r\n" | nc localhost 8080 // cmd for manually writing requests

class Request {
	private:
		string									method;
		string									targetPath;
		string									targetQuery;
		string									httpVersion;
		string									prvsFieldName;
		unordered_map<string, string>			headers;
		stack<bool (Request::*)(stringstream&)>	parseFunctions;
		stack<void (Request::*)(string&)>		parseFunctionsStarterLine;
		string									remainingBuffer;
		e_requestState							state;

		Cgi*									cgi;
		

		vector<string>							splitStarterLine(string& str);
		void									isProtocole(string& httpVersion);
		bool									isCGI(const string& uri);
		void									reconstructAndParseUri(string& uri);
		void									isTarget(string& target);
		void									isMethod(string& method);
		bool									parseStartLine(stringstream&);
		bool									validFieldName(string& str) const;
		bool									parseFileds(stringstream&);
		int										openTargetFile() const;
		bool									contentLengthBased(stringstream&);
		bool									transferEncodingChunkedBased(stringstream&);
		bool									parseBody(stringstream&);
	public:
		Request();
		void									parseMessage(const int clientFd);
		const string&							getMethod()	const;
		const string&							getTarget()	const;
		const string&							getHttpProtocole()	const;
		const string							getHeader(const string&);
		const string&							getPath() const;
		const string&							getQuery() const;
		const t_requestState&					getRequestStatus() const;
};
