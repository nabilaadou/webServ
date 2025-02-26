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
#include "confiClass.hpp"
#include "binarystring.hpp"
#include "stringManipulation.h"
#include "statusCodeException.hpp"
// echo -e "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n" | nc localhost 8080
// host-spawn
#define BUFFER_SIZE 8192
#define URI_MAXSIZE 1024
#define HEADER_FIELD_MAXSIZE 5120

using namespace std;

enum e_sstat {//session stat
	method=0,
	uri,
	httpversion,
	starterlineNl,
	fieldLine,
	wssBeforeFieldName,
	filedName,
	fieldNl,
	emptyline,
	bodyFormat,
	contentLengthBased,
	transferEncodingChunkedBased,
	sHeader,
	sBody,
	done,
	CCLOSEDCON,
};

enum e_requestStat {
	headers,
	body,
};

// struct componentlength {
// 	size_t	s_method;
// 	size_t	s_uri;
// 	size_t	s_httpversion;
// 	size_t	s_starterlineNl;
// 	size_t	s_headerfields;
// 	size_t	s_field;
// 	size_t	s_headersEnd;
// 	size_t	s_bodyLine;

// 	componentlength() : 
// 		s_method(0), s_uri(0), s_httpversion(0)
// 		, s_starterlineNl(0), s_headerfields(0)
// 		, s_field(0) , s_headersEnd(0), s_bodyLine(0) {}
// };

class httpSession {
private:
	e_sstat				sstat;
	e_methods			method;
	string				path;
	string				query;
	map<string, string>	headers;
	int					statusCode;
	string				codeMeaning;
	Cgi*				cgi;
	location*			rules;
	configuration*		config;
public:
	class Request {
	private:
		httpSession&	s;
		e_requestStat	requestStat;
		string			boundary;
		size_t			length;
		int				fd;

		int				parseStarterLine(const bstring& buffer);
		int				parseFields(const bstring& buffer, size_t pos);
		void			parsebody(const bstring& buffer, size_t pos);
		// void			isCGI();
		void			reconstructUri();
		void			getConfigFileRules();
		void			extractPathQuery(const bstring& rawUri);
		void			parseBody();
	public:
		void			readfromsock(const int clientFd);
		Request(httpSession& session);
	};

	class Response {
	private:
		httpSession&	s;
		int				contentFd;
		
		static string	getSupportedeExtensions(const string&);
		string			contentTypeHeader() const;
		void			sendHeader(const int);
		void			sendBody(const int);
		void			sendCgiStarterLine(const int);
		void			sendCgiOutput(const int);
	public:
		Response(httpSession& session);
		void			sendResponse(const int clientFd);
	};

	Request			req;
	Response		res;

	const e_sstat&	status() const;
	void			reSetPath(const string& newPath);
	httpSession(int clientFd, configuration* confi);
	httpSession();

};