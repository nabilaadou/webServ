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



// req
// res

class httpSession {
private:
	string				method;
	string				path;
	string				query;
	string				httpProtocole;
	map<string, string>	headers;
	int					statusCode;
	string				codeMeaning;
	Cgi*				cgi;
public:
	class Request {
	private:
		httpSession&							s;
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
		httpSession&	s;
		int				contentFd;
		t_state			state;

		static string	getSupportedeExtensions(const string&);
		string			contentTypeHeader() const;
		void			sendHeader(const int);
		void			sendBody(const int);
		void			sendCgiStarterLine(const int);
		void			sendCgiOutput(const int);
	public:
		Response(httpSession& session);
		void			sendResponse(const int clientFd);
		const t_state&	status() const;
	};

	httpSession();
	// ~httpSession();
	Request		req;
	Response	res;
};