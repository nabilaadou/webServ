#pragma once
#include <map>
#include <stack>
#include <queue>
#include <vector>
#include "cgi.hpp"
#include <fcntl.h>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <algorithm>
#include "statusCodeException.hpp"

#define BUFFER_SIZE 8192

using namespace std;

class httpSession {
private:
	string				method;
	string				path;
	string				query;
	string				httpProtocole;
	map<string, string>	headers;
	Cgi*				cgi;
public:
	class Request {
	private:
		httpSession&							session;
		string									prvsFieldName;
		queue<bool(Request::*)(stringstream&)>	parseFunctions;
		queue<void(Request::*)(string&)>		parseFunctionsStarterLine;
		int										length;
		int										fd;
		string									remainingBuffer;
		t_state									state;
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
		Request(httpSession& session);
		void									parseMessage(const int clientFd);
		const t_state&							status() const;
	};

	class Response {
	private:
		httpSession&	session;
		int				contentFd;
		int				statusCode;
		string			codeMeaning;
		t_state			state;
		static string	getSupportedeExtensions(const string&);
		string			contentTypeHeader() const;
		void			sendHeader(const int);
		void			sendBody(const int);
		void			sendCgiStarterLine(const int);
		void			sendCgiOutput(const int);
	public:
		Response(httpSession& session);
		const t_state&	status() const;
	};

	httpSession();
	// Request		createRequest();
	// Response	createResponse();
	Request		req;
	Response	res;
};