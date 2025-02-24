#pragma once
#include <map>
#include <stack>
#include <queue>
#include <vector>
#include "cgi.hpp"
#include <fcntl.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <algorithm>
#include <sys/stat.h>
#include "wrappers.h"
#include <sys/socket.h>
#include "binarystring.hpp"
#include "stringManipulation.h"
#include "statusCodeException.hpp"
// echo -e "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n" | nc localhost 8080

#define BUFFER_SIZE 8192
#define URI_MAXSIZE 1024
#define HEADER_FIELD_MAXSIZE 5120

using namespace std;

enum e_methods {
	GET,
	POST,
	DELETE,
};

enum e_headersStat {
	fieldLine = 0,
	filedName,
	nl,
};

enum e_sstat {//session stat
	method=0,
	spBeforeUri,
	uri,
	httpversion,
	starterlineNl,
	headers,
};

struct componentlength {
	size_t	s_method;
	size_t	s_uri;
	size_t	s_httpversion;
	size_t	s_starterlineNl;
	size_t	s_headerfields;

	componentlength() : s_method(0), s_uri(0), s_httpversion(0), s_starterlineNl(0), s_headerfields(0) {}
};

struct location {
	string				uri;
    vector<e_methods>	methods;
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
	e_methods			method;
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
		httpSession&	s;
		e_sstat			stat;

		void			parseRequest(const char* buffer, const size_t size);
	public:
		void			readfromsock(const int clientFd);
		Request(httpSession& session);
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