#pragma once
#include <map>
#include <sstream>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include "statusCodeException.hpp"
#include "cgi.hpp"

#define BUFFER_SIZE 8192

using namespace std;

class Response {
	private:
		string			methode;
		string			target;
		string			httpProtocol;
		int				contentFd;
		Cgi*			cgi;
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
		Response();
		void			sendResponse(const int clinetFd);
		void			equipe(const string&, const string&, const string&, Cgi*);
		void			setStatusCode(const int code, const string& meaning);
		const t_state&	responseStatus() const;
};