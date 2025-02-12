#pragma once
#include <map>
#include <string>
#include <vector>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

using namespace std;

struct cgiInfo {
	string	scriptUri;
	string	scriptName;
	string	exec;
	string	path;
	string	query;
};

class Cgi {
	private:
		const cgiInfo&	infos;
		int				wPipe[2];
		int				rPipe[2];

		void	createPipes();
		// void	executeScript();
		// void    prepearingCgiEnvVars(Request, map<string, string>&);
		// Cgi();
	public:
		Cgi(const cgiInfo& infos);
		// void	setupCGIProcess();
		int		wFd();
		int		rFd();
};