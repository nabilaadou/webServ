#include "httpSession.hpp"

const vector<string> aliasScript= {"/bin/cgi/", "test/test1/"};
const vector<string> addHandler= {".sh", ".py", ".cgi"};

vector<string>	split(string& str) {
	const string	whiteSpace = " \t\n\r\f\v";
	string			remainingStr;
	int				pos = 0, i = 0;
	vector<string>	strings;

	while (str[i]) {
		if (whiteSpace.find(str[i]) != string::npos) {
			strings.push_back(str.substr(pos, i - pos));
			while (str[i] && whiteSpace.find(str[i]) != string::npos)	++i;
			pos = i;
		}
		else ++i;
	}
	while (str.begin()+pos != str.end()) {
		remainingStr += *(str.begin()+pos); //movin the string to point to what i ll add to the remaining buffer;
		++pos;
	}
	str = remainingStr;
	return strings;
}

bool	httpSession::Request::isCGI(const string& uri) {
	stringstream	ss(uri);
	vector<string>	strings;
	string			reappendedUri;
	string			line;

	while (getline(ss, line, '/')) {
		if (!line.empty())
			strings.push_back(line);
	}
	for (const auto& it : aliasScript) {
		if (strncmp(uri.c_str(), it.c_str(), it.size()) == 0) {
			s.cgi = new Cgi();

			size_t pathEndpos = uri.find('/', it.size());
			s.cgi->setScriptPath(uri.substr(0, pathEndpos));
			s.cgi->setScriptName(uri.substr(it.size(), pathEndpos-it.size()));

			size_t queryStartpos = uri.find('?', pathEndpos+1);
			if (pathEndpos < uri.size())
				s.cgi->setPath(uri.substr(pathEndpos+1, queryStartpos-(pathEndpos+1)));
			if (queryStartpos != string::npos)
				s.cgi->setQuery(uri.substr(queryStartpos+1));
			return true;
		}
	}
	for (int i = 0; i < strings.size(); ++i) {
		reappendedUri += "/" + strings[i];
		for (const auto& it : addHandler) {
			if (reappendedUri.rfind(it) != string::npos) {
				s.cgi = new Cgi();

				s.cgi->setScriptPath(reappendedUri);
				s.cgi->setScriptName(strings[i]);

				size_t queryStartpos = uri.find('?', reappendedUri.size());
				if (reappendedUri.size() < uri.size())
					s.cgi->setPath(uri.substr(reappendedUri.size()+1, queryStartpos-(reappendedUri.size()+1)));
				if (queryStartpos != string::npos)
					s.cgi->setQuery(uri.substr(queryStartpos+1));
				return true;
			}
		}
	}
	return false;
}

void	httpSession::Request::reconstructAndParseUri(string& uri) {
	const string root = "./www";
	if (uri[0] != '/') { //removing the scheme and authority and leaving just the path and query
		size_t pos = uri.find('/', 7);
		if (pos == string::npos) {
			uri = "/"; 
			s.path = uri;
			return ;
		}
		else
			uri = uri.substr(pos);
	}
	uri = root + uri;
	if (!isCGI(uri)) {
		size_t pos = uri.find('?');
		s.path = uri.substr(0, pos);
		if (pos != string::npos)
			s.query = uri.substr(pos+1);
		//append the path of the files to the path (eg: /where -> /usr/nabil/Desktop/webserv/www/where) // if path == "/" append to the default path
		if (access(s.path.c_str(), F_OK))	
		{
			if (errno == ENOENT)
				throw(statusCodeException(404, "Not Found"));
			perror("acces failed: ");
			throw(statusCodeException(500, "Internal Server Error"));
		}
	}
}

void	httpSession::Request::isProtocole(string& http) {
	if (http == "HTTP/1.1") {
		s.httpProtocole = http;
		return ;
	}
	else if (http.size() == 8 && !strncmp(http.c_str(), "HTTP/", 5) && isdigit(http[5]) && http[6] == '.' && isdigit(http[7]))
		throw(statusCodeException(505, "HTTP Version Not Supported"));
	throw(statusCodeException(400, "Bad Request"));
}

void	httpSession::Request::isTarget(string& target) {
	const string	validCharachters = "-._~:/?#[]@!$&'()*+,;=";

	if (strncmp(target.c_str(), "http://", 7) && target[0] != '/') {
		if (strncmp(target.c_str(), "https://", 8))
			throw(statusCodeException(501, "Not Implemented"));
		throw(statusCodeException(400, "Bad Request"));
	}
	for (const auto& c : target) {
		if (!iswalnum(c) && validCharachters.find(c) == string::npos)
			throw(statusCodeException(400, "Bad Request"));
	}
	reconstructAndParseUri(target);
}

void	httpSession::Request::isMethod(string& method) {
	if (method == "GET" || method == "POST" || method == "DELETE")
		s.method = method;
	else
		throw(statusCodeException(400, "Bad Request"));
}

bool	httpSession::Request::parseStartLine(stringstream& stream) {
	bool						lineEndedWithLF = false;
	string						line;
	vector<string>				startLineComps;
	vector<string>::iterator	startLineCompsIt;

	getline(stream, line);
	if (!stream.eof()) {
		line += '\n';
		lineEndedWithLF = true;
	}
	startLineComps = split(line);
	if (startLineComps.empty())
		return false;
	if (!lineEndedWithLF)	remainingBuffer = line;
	startLineCompsIt = startLineComps.begin();
	while(!parseFunctionsStarterLine.empty()) {
		const auto& func = parseFunctionsStarterLine.front();
		(this->*func)(*startLineCompsIt);
		parseFunctionsStarterLine.pop();
		if (parseFunctionsStarterLine.empty())
			break;
		if (++startLineCompsIt == startLineComps.end())
			return false;
	}
	if (++startLineCompsIt != startLineComps.end() || !lineEndedWithLF)
		throw(statusCodeException(400, "Bad Request"));
	return true;
}