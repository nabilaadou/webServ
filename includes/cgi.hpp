#pragma once
#include <string>
#include <vector>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <map>

using namespace std;
class Request;

class Cgi {
	private:
		string	scriptPath;
		string	scriptExecuter;
		string	scriptName;
		string	path;
		string	query;
		int		wPipe[2];
		int		rPipe[2];

		void	createPipes();
		void	executeScript();
		// void    prepearingCgiEnvVars(Request, map<string, string>&);
	public:
		Cgi();

		void	setupCGIProcess();
		void	setScriptPath(const string&);
		void	setScriptName(const string&);
		void	setPath(const string&);
		void	setQuery(const string&);
		int		wFd();
		int		rFd();
};