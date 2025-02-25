#include "httpSession.hpp"

void	httpSession::Request::isCGI() {
	size_t		pos = 0;
	cgiInfo		cgiVars;
	bool		foundAMatch = false;

	while (1) {
		pos = s.path.find('/', pos);
		string subUri = s.path.substr(0, pos);
		// if (subUri == s.rules->alias || (subUri + '/') == loc->uri)
		// 	subUri += '/' + loc->index;
		size_t	dotPos = subUri.rfind('.');
		string subUriExt = "";
		if (dotPos != string::npos)
			subUriExt = subUri.substr(dotPos);
		if (s.rules->cgis.find(subUriExt) != s.rules->cgis.end() && !access(("." + subUri).c_str() ,F_OK)) {
			cgiVars.scriptUri = w_realpath(("." + subUri).c_str());
			size_t barPos = subUri.rfind('/');
			cgiVars.scriptName = subUri.substr(barPos+1);
			cgiVars.exec = s.rules->cgis[subUriExt];
			if (s.path.size() > subUri.size()+1)
				cgiVars.path = s.path.substr(pos);
			cgiVars.query = s.query;
			foundAMatch = true;
		}
		if (pos++ == string::npos)
			break;
	}
	if (foundAMatch == true)
		s.cgi = new Cgi(cgiVars);
}

void	httpSession::Request::reconstructUri() {
	struct stat pathStat;
//use switch cases
	if (find(s.rules->methods.begin(), s.rules->methods.end(), s.method) == s.rules->methods.end())
		throw(statusCodeException(405, "Method Not Allowed"));
	switch (s.method)
	{
	case GET: {
		if (s.rules->redirection) {
			s.statusCode = 301;
			s.codeMeaning = "Moved Permanently";
			//adding the location header to the response with the new path;
			return ;
		} else {
			s.path.erase(s.path.begin(), s.path.begin()+s.rules->uri.size() - 1);
			s.path = s.rules->reconfigurer + s.path;
		}
		break;
	}
	case POST: {
		if (!s.rules->uploads.empty()) {
			s.path = s.rules->uploads;
			s.path = w_realpath(("." + s.path).c_str());
			if (stat(s.path.c_str(), &pathStat) && !S_ISDIR(pathStat.st_mode))
				throw(statusCodeException(403, "Forbidden"));
		} else
			throw(statusCodeException(403, "Forbidden"));
		break;
	}
	case DELETE: {
		if (!s.rules->uploads.empty()) {
			s.path = s.rules->uploads + s.path;
			s.path = w_realpath(("." + s.path).c_str());
		} else
			throw(statusCodeException(403, "Forbidden"));
	}
	}
	s.path = w_realpath(("." + s.path).c_str());
    stat(s.path.c_str(), &pathStat);
	if (S_ISDIR(pathStat.st_mode)) {//&& s->path == location
		s.path += "/" + s.rules->index;
		if (stat(s.path.c_str(), &pathStat))
		throw(statusCodeException(404, "Not Found"));
	}
	// isCGI();
}

void	httpSession::Request::getConfigFileRules() {
	size_t		pos = 0;

	if (s.config->locations.find(s.path) != s.config->locations.end()) {
		s.rules = &(s.config->locations.at(s.path));
		return;
	}
	while (1) {
		pos = s.path.find('/', pos);
		string subUri = s.path.substr(0, pos+1);
		if (s.config->locations.find(subUri) != s.config->locations.end())
			s.rules = &(s.config->locations.at(subUri));
		if (pos++ == string::npos)
			break;
	}
	if (s.rules == NULL)
		throw(statusCodeException(404, "Not Found"));
}


void	httpSession::Request::extractPathQuery(const bstring rawUri) {
	cerr << "raww: " << rawUri << endl;
	size_t pos = rawUri.find('?');
	s.path = rawUri.substr(0, pos).cppstring();
	if (pos != string::npos)
		s.query = rawUri.substr(pos+1).cppstring();
}


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
		switch (s.sstat) {
		case e_sstat::method: {
			switch (xlength.s_method)
			{
			case 2:{
				if (!buffer.ncmp("GET", 3)) {
					s.sstat = spBeforeUri;
					s.method = GET;
				}
				break;
			}
			case 3: {
				if (!buffer.ncmp("POST", 4)) {
					s.sstat = spBeforeUri;
					s.method = POST;
				}
				break;
			}
			case 5: {
				if (!buffer.ncmp("DELETE", 6)) {
					s.sstat = spBeforeUri;
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
			s.sstat = uri;
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
						// if (buffer[i-1] != '/')
						// 	throw(statusCodeException(400, "Bad Requestss"));
						extractPathQuery(buffer.substr(i-xlength.s_uri, xlength.s_uri));
						getConfigFileRules();
						reconstructUri();
						cerr << s.path << endl;
						// exit(0);
						s.sstat = e_sstat::httpversion;
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
				s.sstat = e_sstat::starterlineNl;
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
				s.sstat = e_sstat::emptyLine;
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
				s.sstat = e_sstat::wssBeforeFieldName;
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
				s.sstat = e_sstat::fieldNl;
				break;
			}
			case '\n':
				s.sstat = e_sstat::emptyLine;
			case ' ':
			case '\t':
			case '\f':
			case '\v':
				break;
			default: {
				--i;
				s.sstat = e_sstat::filedName;
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
				s.sstat = e_sstat::fieldNl;
				break;
			}
			case '\n': {
				fieldname = buffer.substr(i-xlength.s_field, xlength.s_field).cppstring();
				s.sstat = e_sstat::emptyLine;
			}
			}
			++xlength.s_field;
			++xlength.s_headerfields;
			break ;
		}
		case e_sstat::fieldNl: {
			if (ch != '\n')
				throw(statusCodeException(400, "Bad Request"));
			s.sstat = e_sstat::emptyLine;
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
				switch (s.method)
				{
					case POST: {
						s.sstat = e_sstat::body;
						break;
					}
					default:
						s.sstat = e_sstat::sHeader;
				}
				if (!fieldline.empty())
					s.headers[fieldline] = fieldname;
				break;
			}
			default: {
				if (xlength.s_headersEnd != 0)
					throw(statusCodeException(400, "Bad Request"));
				--i;//goin back to the prev index so i can check if the char is valid in fieldname terms
				s.sstat = e_sstat::fieldLine;
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
}
