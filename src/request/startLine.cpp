#include "httpSession.hpp"

// inline void	cmpget(char c1, char c2, char c3) {
// 	if (c1 == 'G' && c2 == 'E' && c3 == 'T')
// 		return ;
// 	throw(statusCodeException(400, "Bad Request"));
// }

// inline void	cmppost(char c1, char c2, char c3, char c4) {
// 	if (c1 == 'P' && c2 == 'O' && c3 == 'S' && c4 == 'T')
// 		return ;
// 	throw(statusCodeException(400, "Bad Request"));
// }

// inline void	cmpdelete(char c1, char c2, char c3, char c4, char c5, char c6) {
// 	if (c1 == 'D' && c2 == 'E' && c3 == 'L' && c4 == 'E' && c5 == 'T' && c6 == 'E')
// 		return ;
// 	throw(statusCodeException(400, "Bad Request"));
// }

// static vector<string>	split(string& str) {
// 	const string	whiteSpace = " \t\n\r\f\v";
// 	string			remainingStr;
// 	int				pos = 0, i = 0;
// 	vector<string>	strings;

// 	while (str[i]) {
// 		if (whiteSpace.find(str[i]) != string::npos) {
// 			strings.push_back(str.substr(pos, i - pos));
// 			while (str[i] && whiteSpace.find(str[i]) != string::npos)	++i;
// 			pos = i;
// 		}
// 		else ++i;
// 	}
// 	while (str.begin()+pos != str.end()) {
// 		remainingStr += *(str.begin()+pos);
// 		++pos;
// 	}
// 	str = remainingStr;
// 	return strings;
// }

// void	httpSession::Request::isCGI(location* loc) {
// 	size_t		pos = 0;
// 	cgiInfo		cgiVars;
// 	bool		foundAMatch = false;

// 	while (1) {
// 		pos = s.path.find('/', pos);
// 		string subUri = s.path.substr(0, pos);
// 		if (subUri == loc->alias || (subUri + '/') == loc->uri)
// 			subUri += '/' + loc->index;
// 		size_t	dotPos = subUri.rfind('.');
// 		string subUriExt = "";
// 		if (dotPos != string::npos)
// 			subUriExt = subUri.substr(dotPos);
// 		if (loc->cgi.find(subUriExt) != loc->cgi.end() && !access(("." + subUri).c_str() ,F_OK)) {
// 			cgiVars.scriptUri = w_realpath(("." + subUri).c_str());
// 			size_t barPos = subUri.rfind('/');
// 			cgiVars.scriptName = subUri.substr(barPos+1);
// 			cgiVars.exec = loc->cgi[subUriExt];
// 			if (s.path.size() > subUri.size()+1)
// 				cgiVars.path = s.path.substr(pos);
// 			cgiVars.query = s.query;
// 			foundAMatch = true;
// 		}
// 		if (pos++ == string::npos)
// 			break;
// 	}
// 	if (foundAMatch == true)
// 		s.cgi = new Cgi(cgiVars);
// }

// void	httpSession::Request::reconstructUri(location*	rules) {
// 	struct stat pathStat;

// 	if (find(rules->methods.begin(), rules->methods.end(), s.method) == rules->methods.end())
// 		throw(statusCodeException(405, "Method Not Allowed"));
// 	if (s.method == GET) {
// 		if (!rules->redirection.empty()) {
// 			s.statusCode = 301;
// 			s.codeMeaning = "Moved Permanently";
// 			//adding the location header to the response with the new path;
// 			return ;
// 		}
// 		if (!rules->alias.empty()) {
// 			s.path.erase(s.path.begin(), s.path.begin()+rules->uri.size() - 1);
// 			s.path = rules->alias + s.path;
// 		}
// 	} else if (s.method == POST) {
// 		if (!rules->upload.empty()) {
// 			s.path = rules->upload;
// 			s.path = w_realpath(("." + s.path).c_str());
// 			if (stat(s.path.c_str(), &pathStat) && !S_ISDIR(pathStat.st_mode))
// 				throw(statusCodeException(403, "Forbidden"));
// 			return ;
// 		} else
// 			throw(statusCodeException(403, "Forbidden"));
// 	}
// 	isCGI(rules);
// 	if (s.cgi == NULL) {
// 		s.path = w_realpath(("." + s.path).c_str());
//     	if (stat(s.path.c_str(), &pathStat))
// 			throw(statusCodeException(404, "Not Found"));
// 		if (S_ISDIR(pathStat.st_mode)) {//&& s->path == location
// 			s.path += "/" + rules->index;
// 			if (stat(s.path.c_str(), &pathStat))
// 				throw(statusCodeException(404, "Not Found"));
// 		}
// 	}
// }

// location*	httpSession::Request::getConfigFileRules() {
// 	size_t		pos = 0;
// 	location*	loc = NULL;

// 	if (s.config->locations.find(s.path) != s.config->locations.end()) {
// 		loc = &(s.config->locations.at(s.path));
// 		return loc;
// 	}
// 	while (1) {
// 		pos = s.path.find('/', pos);
// 		string subUri = s.path.substr(0, pos+1);
// 		if (s.config->locations.find(subUri) != s.config->locations.end())
// 			loc = &(s.config->locations.at(subUri));
// 		if (pos++ == string::npos)
// 			break;
// 	}
// 	return loc;
// }

// void	httpSession::Request::isProtocole(bstring& http) {
// 	if (!http.cmp("HTTP/1.1")) {
// 		s.httpProtocole = http.cppstring();
// 		return ;
// 	}
// 	else if (http.size() == 8 && http.ncmp("HTTP/", 5) == 0 && isdigit(http[5]) && http[6] == '.' && isdigit(http[7]))
// 		throw(statusCodeException(505, "HTTP Version Not Supported"));
// 	throw(statusCodeException(400, "Bad Request"));
// }

// void	httpSession::Request::extractPathQuery(bstring& uri) {
// 	if (uri[0] != '/') {
// 		size_t pos = uri.find("/", 7);
// 		if (pos == string::npos)
// 			uri = "/";
// 		else
// 			uri = uri.substr(pos);
// 	}
// 	size_t pos = uri.find("?");
// 	s.path = uri.substr(0, pos).cppstring();
// 	if (pos != string::npos)
// 		s.query = uri.substr(pos+1).cppstring();
// }

// void	httpSession::Request::isTarget(bstring& target) {
// 	const string	validCharachters = "-._~:/?#[]@!$&'()*+,;=";

// 	if (target.ncmp("http://", 7) && target[0] != '/')
// 		throw(statusCodeException(400, "Bad Request"));
// 	for (size_t i = 0; i < target.size(); ++i) {
// 		if (!iswalnum(target[i]) && validCharachters.find(target[i]) == string::npos)
// 			throw(statusCodeException(400, "Bad Request"));
// 	}
// 	extractPathQuery(target);
// }

// void	httpSession::Request::isMethod(bstring& method) {
// 	switch (method.size())
// 	{
// 	case 5: {
// 		cmpdelete(method[0], method[1], method[2], method[3], method[4], method[5]);
// 		s.method = DELETE;
// 		break;
// 	}
// 	case 4: {
// 		cmppost(method[0], method[1], method[2], method[3]);
// 		s.method = POST;
// 		break;
// 	}
// 	case 3: {
// 		cmpget(method[0], method[1], method[2]);
// 		s.method = GET;
// 		break;
// 	}
// 	default:
// 		throw(statusCodeException(400, "Bad Request"));
// 	}
// }


// bool	httpSession::Request::parseStartLine(bstring& buffer) {
// 	bstring		line;
// 	bool		eof;
// 	location*	rules;
// 	char		absolutePath[1024];

// 	if (buffer.getheaderline(line)) {
// 		vector<bstring>	list = line.split();
// 		if (list.size() != 3)
// 			throw(statusCodeException(400, "Bad Request"));
// 		isMethod(list[0]);
// 		isTarget(list[1]);
// 		isProtocole(list[2]);
// 		if ((rules = getConfigFileRules()))
// 			reconstructUri(rules);
// 		else
// 			throw(statusCodeException(404, "Not Found"));
// 		return true;
// 	}
// 	remainingBuffer = line;//GET / HTTP\0
// 	return false;
// }

void	httpSession::Request::parseRequest(bstring& buffer) {
	char			ch;
	size_t			size = buffer.size();
	string			fieldline;
	string			fieldname;
	componentlength xlength;

	for (size_t i = 0; i < size; ++i) {
		ch = buffer[i];
		//component are seperated by a SINGLE space
		switch (s.stat) {
		case e_sstat::method: {
			switch (xlength.s_method)
			{
			case 2:{
				if (!buffer.ncmp("GET", 3)) {
					s.stat = spBeforeUri;
					s.method = GET;
				}
				break;
			}
			case 3: {
				if (!buffer.ncmp("POST", 4)) {
					s.stat = spBeforeUri;
					s.method = POST;
				}
				break;
			}
			case 5: {
				if (!buffer.ncmp("DELETE", 6)) {
					s.stat = spBeforeUri;
					s.method = DELETE;
				}
				break;
			}
			case 6:
				throw(statusCodeException(400, "Bad Request"));
			}
			++xlength.s_method;
			break;
		}
		case e_sstat::spBeforeUri: {
			if (ch != ' ')
				throw(statusCodeException(400, "Bad Request"));
			s.stat = uri;
			break;
		}
		case e_sstat::uri: {
			switch (xlength.s_uri)
			{
			case 0: {
				if (ch != '/')
					throw(statusCodeException(400, "Bad Request"));
				break;
			}
			case URI_MAXSIZE:
				throw(statusCodeException(414, "URI Too Long"));
			default: {
				switch (ch)
				{
					case ' ': {
						rawUri = buffer.substr(i-xlength.s_uri, xlength.s_uri).cppstring();
						s.stat = e_sstat::httpversion;
						break;
					}
					case '-':
					case '.':
					case '_':
					case '~':
					case ':':
					case '/':
					case '?':
					case '#':
					case '[':
					case ']':
					case '@':
					case '!':
					case '$':
					case '&':
					case '\'':
					case '(':
					case ')':
					case '*':
					case '+':
					case ',':
					case ';':
					case '=':
						break;
					default: {
						if (!iswalnum(ch))
							throw(statusCodeException(400, "Bad Request"));
					}
				}
			}
			}
			++xlength.s_uri;
			break;
		}
		case e_sstat::httpversion: {
			switch (xlength.s_httpversion)
			{
			case 4: {
				if (!buffer.ncmp("HTTP/", 5, i-4) == false)
					throw(statusCodeException(400, "Bad Request"));
				break;
			}
			case 5: {
				if (ch != '1')
					throw(statusCodeException(400, "Bad Request"));
				break;
			}
			case 6: {
				if (ch != '.')
					throw(statusCodeException(400, "Bad Request"));
				break;
			}
			case 7: {
				if (ch != '1')
					throw(statusCodeException(400, "Bad Request"));
				s.stat = e_sstat::starterlineNl;
				break;
			}
			}
			++xlength.s_httpversion;
			break;
		}
		case e_sstat::starterlineNl: {
			switch (ch)
			{
			case '\r': {
				if (xlength.s_starterlineNl != 0)
					throw(statusCodeException(400, "Bad Request"));
				break;
			}
			case '\n': {
				if (xlength.s_starterlineNl > 1)
					throw(statusCodeException(400, "Bad Request"));
				s.stat = e_sstat::emptyLine;
				break;
			}
			default:
				throw(statusCodeException(400, "Bad Request"));
			}
			++xlength.s_starterlineNl;
			break;
		}
		case e_sstat::fieldLine: {
			switch (ch)
			{
			case ':': { //test-> : field name ? valid?
				fieldline = buffer.substr(i-xlength.s_field, xlength.s_field).cppstring();
				s.stat = e_sstat::wssBeforeFieldName;
				break;
			}
			case '-':
			case '_':
				break;
			default: {
				if (!iswalnum(ch))
					throw(statusCodeException(400, "Bad Request"));
			}
			}
			++xlength.s_field;
			++xlength.s_headerfields;
			break;
		}
		case e_sstat::wssBeforeFieldName: {
			switch (ch)
			{
			case '\r': {
				s.stat = e_sstat::fieldNl;
				break;
			}
			case '\n':
				s.stat = e_sstat::emptyLine;
			case ' ':
			case '\t':
			case '\f':
			case '\v':
				break;
			default: {
				--i;
				s.stat = e_sstat::filedName;
				xlength.s_field = 0;
				continue;
			}
			}
			++xlength.s_headerfields;
			break;
		}
		case e_sstat::filedName: {
			switch (ch)
			{
			case '\r': {
				fieldname = buffer.substr(i-xlength.s_field, xlength.s_field).cppstring();
				s.stat = e_sstat::fieldNl;
				break;
			}
			case '\n': {
				fieldname = buffer.substr(i-xlength.s_field, xlength.s_field).cppstring();
				s.stat = e_sstat::emptyLine;
			}
			}
			++xlength.s_field;
			++xlength.s_headerfields;
			break ;
		}
		case e_sstat::fieldNl: {
			if (ch != '\n')
				throw(statusCodeException(400, "Bad Request"));
			s.stat = e_sstat::emptyLine;
			++xlength.s_headerfields;
			break;
		}
		case e_sstat::emptyLine: {
			switch (ch)
			{
			case '\r': {
				if (xlength.s_headersEnd != 0)
					throw(statusCodeException(400, "Bad Request"));
				break;
			}
			case '\n': {
				if (xlength.s_headersEnd > 1)
					throw(statusCodeException(400, "Bad Request"));
				s.stat = e_sstat::body;
				if (!fieldline.empty())
					s.headers[fieldline] = fieldname;
				break;
			}
			default: {
				if (xlength.s_headersEnd != 0)
					throw(statusCodeException(400, "Bad Request"));
				--i;//goin back to the prev index so i can check if the char is valid in fieldname terms
				s.stat = e_sstat::fieldLine;
				xlength.s_field = 0;
				if (!fieldline.empty())
					s.headers[fieldline] = fieldname;
				fieldline = "";
				fieldname = "";
				continue;
			}
			}
			++xlength.s_headersEnd;
			++xlength.s_headerfields;
			break;
		}
		}
		if (xlength.s_headerfields > HEADER_FIELD_MAXSIZE)
			throw(statusCodeException(431, "Request Header Fields Too Large"));
	}
	s.stat = CCLOSEDCON;//remove
}
