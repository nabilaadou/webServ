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
#include <sys/stat.h>
#include "wrappers.h"
#include "binarystring.hpp"
#include "stringManipulation.h"
#include "statusCodeException.hpp"
// echo -e "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n" | nc localhost 8080

#define BUFFER_SIZE 8192

using namespace std;

enum methods {
	GET,
	POST,
	DELETE,
};

struct location {
	string				uri;
    vector<methods>		methods;
    string				redirection;
    string				alias;
	string				upload;
    string				index;
	map<string, string>	cgi;
    bool				autoIndex;
	location() : index("index.html") {}
};

struct configuration {
    int						bodySize;
    map<int, string>		errorPages;
    map<string, location>	locations;
};

class httpSession {
private:
	methods				method;
	string				path;
	string				query;
	string				httpProtocole;
	map<string, string>	headers;
	int					statusCode;
	string				codeMeaning;
	Cgi*				cgi;
	configuration*		config;
public:
	class Request {
	private:
		httpSession&							s;
		string									prvsFieldName;
		string									prvsContentFieldName;
		queue<bool(Request::*)(bstring&)>		parseFunctions;
		queue<bool(Request::*)(string&)>		bodyParseFunctions;
		map<string, string>						contentHeaders;
		int										length;
		int										fd;
		string									boundaryValue;
		// string									remainingBuffer;
		t_state									state;

		void									isCGI(location*);
		void									reconstructUri(location* rules);
		void									isProtocole(bstring& httpVersion);
		void									extractPathQuery(bstring& uri);
		void									isTarget(bstring& target);
		void									isMethod(bstring& method);
		location*								getConfigFileRules();
		bool									parseStartLine(bstring&);
		bool									validFieldName(string& str) const;
		bool									parseFileds(bstring&);
		int										openTargetFile(const string& filename) const;
		bool									boundary(string&);
		bool									fileHeaders(string&);
		bool									fileContent(string&);
		bool									contentLengthBased(bstring&);
		bool									transferEncodingChunkedBased(bstring&);
		bool									parseBody(bstring&);
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

	Request		req;
	Response	res;

	httpSession(int clientFd, configuration* confi);
	httpSession();
	void		reSetPath(const string& newPath);

};