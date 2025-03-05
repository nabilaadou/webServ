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
	unchunkBody,
	writeToCgiStdin,
	sHeader,
	sBody,
	done,
	CCLOSEDCON,
};

enum e_requestStat {
	headers,
	body,
};

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
	bstring				unchunkedBody;
	location*			rules;
	configuration*		config;
public:
	class Request {
	private:
		httpSession&	s;
		e_requestStat	requestStat;
		bstring			remainingBody;
		string			boundary;
		size_t			length;
		int				fd;

		int				parseStarterLine(const bstring& buffer);
		void			parseBody(const bstring& buffer, size_t pos);
		void			isCGI();
		void			reconstructUri();
	public:
		void			readfromsock(const int clientFd);
		Request(httpSession& session);
	};

	class Response {
	private:
		httpSession&		s;
		int					contentFd;
		bool				cgiHeadersParsed;
		
		static string	getSupportedeExtensions(const string&);
		string			contentTypeHeader() const;
		void			sendHeader(const int);
		void			sendBody(const int);
		void			sendCgiStarterLine(const int);
		void			sendCgiOutput(const int);
	public:
		void			sendResponse(const int clientFd);
		Response(httpSession& session);
	};

	Request			req;
	Response		res;

	int				parseFields(const bstring& buffer, size_t pos, map<string, string>& headers);
	const e_sstat&	status() const;
	void			reSetPath(const string& newPath);
	httpSession(int clientFd, configuration* confi);
	httpSession();

};