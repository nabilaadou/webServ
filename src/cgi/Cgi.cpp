/*
	fork and run teh script and pipe and read it and ship it
	before check if the headers are well formed and make the start-line and send the response
*/

//https://www.ibm.com/docs/en/netcoolomnibus/8.1?topic=scripts-environment-variables-in-cgi-script

#include "cgi.hpp"


Cgi::Cgi(): rPipe({-1}), wPipe({-1}){
	createPipes();
}

void	Cgi::createPipes() {
	if (pipe(wPipe) < 0 || pipe(rPipe) < 0) {
		perror("pipe failed"); exit(-1);
	}
}
















// void	Cgi::prepearingCgiEnvVars(Request req, map<string, string>& mapEnvp) {
// 	mapEnvp["GATEWAY_INTERFACE"] = "CGI/1.1";//idk
// 	mapEnvp["SERVER_PROTOCOL"] = "http/1.1";
// 	mapEnvp["SERVER_NAME"] = "localhost";
// 	mapEnvp["REMOTE_METHODE"] = req.getMethod();
// 	mapEnvp["PATH_INFO"] = path;
// 	mapEnvp["QUERY_STRING"] = query;
// 	mapEnvp["SCRIPT_NAME"] = scriptName;
// 	mapEnvp["CONTENT_LENGTH"] = req.getHeader("content-length");
// 	mapEnvp["CONTENT_TYPE"] = req.getHeader("content-type");
// 	mapEnvp["HTTP_ACCEPT"] = req.getHeader("accept");
// 	mapEnvp["HTTP_ACCEPT_CHARSET"] = req.getHeader("accept-charset");
// 	mapEnvp["HTTP_ACCEPT_ENCODING"] = req.getHeader("accept-encoding");
// 	mapEnvp["HTTP_ACCEPT_LANGUAGE"] = req.getHeader("accept-language");
// 	mapEnvp["HTTP_USER_AGENT"] = req.getHeader("user-agent");
// 	// mapEnvp["HTTP_HOST"] = "";//idk
// 	// mapEnvp["PATH_TRANSLATED"] = "";//idk
// 	// mapEnvp["REMOTE_ADDR"] = "";//idk
// 	// mapEnvp["REMOTE_HOST"] = "";
// 	// mapEnvp["REMOTE_USER"] = "";
// 	// mapEnvp["SERVER_PORT"] = "";
// 	// mapEnvp["WEBTOP_USER"] = "";
// }

char**	transformVectorToChar(vector<string>& vec) {
	char** 	CGIEnvp = new char*[vec.size() + 1];

	for (int i = 0; i < vec.size(); ++i) {
		CGIEnvp[i] = new char[vec[i].size()+1];
		strcpy(CGIEnvp[i], vec[i].c_str());
	}
	CGIEnvp[vec.size()] = NULL;
	return (CGIEnvp);
}

void	Cgi::executeScript() {
	char *const*					argv;
	const char* 					path;
	char** 							CGIEnvp = NULL;
	vector<string>					vecArgv;
	vector<string> 					vecEnvp;
	// map<string, string>	mapEnvp;
	// prepearingCgiEnvVars(req, mapEnvp);
	// for(const auto& it: mapEnvp) {
	// 	if (!it.second.empty())
	// 		vecEnvp.push_back(it.first + "=" + it.second);
	// }
	// while(*ncHomeEnvp) {
	// 	vecEnvp.push_back(*ncHomeEnvp);
	// 	++ncHomeEnvp;
	// }
	// CGIEnvp = transformVectorToChar(vecEnvp);
	//exec
	if (!scriptExecuter.empty())
		vecArgv.push_back(scriptExecuter);
	vecArgv.push_back(scriptPath);
	argv = transformVectorToChar(vecArgv);
	path = (scriptExecuter.empty()) ? scriptPath.c_str() : scriptExecuter.c_str();
	cerr << path << endl;
	int i = 0;
	while(argv[i]) {
		cerr << argv[i] << endl;
		++i;
	}
	exit(0);
	execve(path, argv, CGIEnvp);
}

void	Cgi::setupCGIProcess() {
	// //fd[1] // write end;
	// //fd[0] // read end;
	pid_t pid = fork();
	if (pid < 0) {
		perror("fork failed"); exit(-1);
	}
	else if(pid == 0) {
		close(rPipe[0]);
		close(wPipe[1]);

		if (dup2(rPipe[1], STDOUT_FILENO) < 0 || dup2(wPipe[0], STDIN_FILENO) < 0) {
			perror("dup2 failed"); exit(-1);
		}
		close(rPipe[1]);
		close(wPipe[0]);
		executeScript();
	}
	wait(0);
	close(rPipe[1]);
	close(wPipe[0]);
}