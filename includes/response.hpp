#pragma once
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
#include "Cgi.hpp"
#include "statusCodeException.hpp"

#define BUFFER_SIZE 8192

typedef enum e_responseState{
	PROCESSING_S,
	DONE_S,
	CCLOSEDCON_S,
}	t_responseState;

using namespace std;

string getSupportedeExtensions(const string& key);

class Response {
	private:
		string			methode;
		string			target;
		string			httpProtocol;
		int				contentFd;
		bool			sentHeader;
		int				statusCode;
		string			codeMeaning;
		t_responseState	state;

		string		contentTypeHeader();
		void		sendHeader(const int);
		void		sendBody(const int);
	public:
		Response();
		void			sendResponse(const int clinetFd);
		void			equipe(const string&, const string&, const string&);
		void			setStatusCode(const int code, const string& meaning);
		t_responseState	responseStatus();
};