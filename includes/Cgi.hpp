#pragma once
#include <string>
#include <string.h>
#include <unordered_map>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <vector>

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
		// void    prepearingCgiEnvVars(Request, unordered_map<string, string>&);	
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