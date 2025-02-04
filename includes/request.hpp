#pragma once

#include <vector>
#include <stack>
#include "cgi.hpp"
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <algorithm>
#include <unordered_map>
#include "statusCodeException.hpp"

#define BUFFER_SIZE 8192 

using namespace std;

class Cgi;

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
		t_state									state;

		Cgi*									cgi;
		
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
		const string&							Method()	const;
		const string&							HttpProtocole()	const;
		const string							Header(const string&);
		const string&							Path() const;
		const string&							Query() const;
		Cgi*									cgiPointer();
		const t_state&							RequestStatus() const;
};
