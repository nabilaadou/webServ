#include "cgi.hpp"

Cgi::Cgi(const cgiInfo& infos): infos(infos), rPipe{-1}, wPipe{-1}{
	createPipes();
}

Cgi::~Cgi() {
	cerr << "des called" << endl;
}

void	Cgi::createPipes() {
	if (pipe(wPipe) < 0 || pipe(rPipe) < 0) {
		perror("pipe failed");
		throw(statusCodeException(500, "Internal Server Error"));
	}
}

static string	proccesHeaderValue(const string& key) {
	string proccesedKey = "HTTP_";

	for (int i = 0; i < key.size(); ++i) {
		char ch = toupper(key[i]);
		if (ch == '-')
			ch = '_';
		proccesedKey += ch;
	}
	return proccesedKey;
}

void	Cgi::getHeaders(const map<string, string>& headers) {
	for (map<string, string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
		scriptEnvs[proccesHeaderValue(it->first)] = it->second;
}	

void	Cgi::prepearingCgiEnvVars(const map<string, string>& headers) {
	scriptEnvs["GATEWAY_INTERFACE"] = "CGI/1.1";
	scriptEnvs["SERVER_PROTOCOL"] = "http/1.1";
	scriptEnvs["SERVER_NAME"] = "localhost";
	// scriptEnvs["REMOTE_METHODE"] = req.getMethod();
	scriptEnvs["PATH_INFO"] = infos.path;
	scriptEnvs["QUERY_STRING"] = infos.query;
	scriptEnvs["SCRIPT_NAME"] = infos.scriptName;
	getHeaders(headers);
	// scriptEnvs["PATH_TRANSLATED"] = "";//idk
	for (const auto& it : scriptEnvs)
		cerr << it.first << ": " << it.second << endl;
}

static char**	transformVectorToChar(vector<string>& vec) {
	char** 	strs = new char*[vec.size() + 1];

	for (size_t i = 0; i < vec.size(); ++i) {
		strs[i] = new char[vec[i].size()+1];
		strcpy(strs[i], vec[i].c_str());
	}
	strs[vec.size()] = NULL;
	return strs;
}

// HTTP/1.1 200 OK

void	Cgi::executeScript() {
	char**							argv, CGIEnvp;
	vector<string>					vecArgv, vecEnvp;
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
	if (!infos.exec.empty())
		vecArgv.push_back(infos.exec);
	vecArgv.push_back(infos.scriptUri);
	argv = transformVectorToChar(vecArgv);
	if (execve(argv[0], argv, NULL) == -1) {
		for (size_t i = 0; argv[i]; ++i) {
			delete argv[i];
		}
		delete []argv;
		perror("execve failed(cgi.cpp 102)"); exit(-1);
	}
}

void	Cgi::setupCGIProcess() {
	// //fd[1] // write end;
	// //fd[0] // read end;
	pid = fork();
	if (pid < 0) {
		perror("fork failed");
		throw(statusCodeException(500, "Internal Server Error"));
	}
	else if(pid == 0) {
		close(rPipe[0]);
		close(wPipe[1]);
		if (dup2(rPipe[1], STDOUT_FILENO) < 0 || dup2(wPipe[0], STDIN_FILENO) < 0) {
			perror("dup2 failed");
			throw(statusCodeException(500, "Internal Server Error"));
		}
		close(rPipe[1]);
		close(wPipe[0]);
		executeScript();
	}
	wait(0);//use waitpid and read from the pipe to free space for the write
	close(rPipe[1]);
	close(wPipe[0]);
}