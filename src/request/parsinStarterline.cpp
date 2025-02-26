#include "httpSession.hpp"

static bool	isMultipartFormData(const string& value) {
	vector<string>	fieldValueparts;
	split(value, ';', fieldValueparts);
	if (fieldValueparts.size() != 2)
		return false;
	if (trim(fieldValueparts[0]) != "multipart/form-data")
		return false;
	if (strncmp(trim(fieldValueparts[1]).c_str(), "boundary=", 9))
		return false;
	return true;
}

static int	openFile(const string& value) {
	int				fd;
	vector<string>	fieldValueparts;
	vector<string> keyvalue;

	split(value, ';', fieldValueparts);
	if (fieldValueparts.size() != 3 || strncmp("form-data" ,trim(fieldValueparts[0]).c_str(), 9))
		throw(statusCodeException(501, "Not Implemented"));
	split(trim(fieldValueparts[1]), '=', keyvalue);
	if (keyvalue.size() != 2 || strncmp("name", keyvalue[0].c_str(), 4) || keyvalue[1][0] != '"' || keyvalue[1][keyvalue[1].size()-1] != '"')
		throw(statusCodeException(501, "Not Implemented"));
	keyvalue.clear();
	split(trim(fieldValueparts[2]), '=', keyvalue);
	if (keyvalue.size() != 2 || strncmp("filename", keyvalue[0].c_str(), 8) || keyvalue[1][0] != '"' || keyvalue[1][keyvalue[1].size()-1] != '"')
		throw(statusCodeException(501, "Not Implemented"));
	keyvalue[1].erase(keyvalue[1].begin());
	keyvalue[1].erase(keyvalue[1].end()-1);
	if ((fd = open(keyvalue[1].c_str(), O_WRONLY, 0644)) < 0)
		throw(statusCodeException(500, "Internal Server Error"));
	return fd;
}

// void	httpSession::Request::isCGI() {
// 	size_t		pos = 0;
// 	cgiInfo		cgiVars;
// 	bool		foundAMatch = false;

// 	while (1) {
// 		pos = s.path.find('/', pos);
// 		string subUri = s.path.substr(0, pos);
// 		// if (subUri == s.rules->alias || (subUri + '/') == loc->uri)
// 		// 	subUri += '/' + loc->index;
// 		size_t	dotPos = subUri.rfind('.');
// 		string subUriExt = "";
// 		if (dotPos != string::npos)
// 			subUriExt = subUri.substr(dotPos);
// 		if (s.rules->cgis.find(subUriExt) != s.rules->cgis.end() && !access(("." + subUri).c_str() ,F_OK)) {
// 			cgiVars.scriptUri = w_realpath(("." + subUri).c_str());
// 			size_t barPos = subUri.rfind('/');
// 			cgiVars.scriptName = subUri.substr(barPos+1);
// 			cgiVars.exec = s.rules->cgis[subUriExt];
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

void	httpSession::Request::reconstructUri() {
	struct stat pathStat;

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
			s.path.erase(s.path.begin(), s.path.begin()+s.rules->uri.size());
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

int	httpSession::Request::parseStarterLine(const bstring& buffer) {
	char			ch;
	size_t			size = buffer.size();
	size_t			len = 0;

	for (size_t i = 0; i < size; ++i) {
		ch = buffer[i];
		switch (s.sstat)
		{
		case e_sstat::method: {
			switch (ch)
			{
			case ' ': {
				switch (len)
				{
				case 3: {
					if (buffer.ncmp("GET", 3, i-len))
						throw(statusCodeException(400, "Bad Request"));
					break;
				}
				case 4:{
					if (buffer.ncmp("POST", 4, i-len))
						throw(statusCodeException(400, "Bad Request"));
					break;
				}
				case 6:{
					if (buffer.ncmp("DELETE", 6, i-len))
						throw(statusCodeException(400, "Bad Request"));
				}
				}
				s.sstat = e_sstat::uri;
				len = 0;
				continue;
			}
			default: {
				if (len > 6)
					throw(statusCodeException(400, "Bad Request"));
			}
			}
			++len;
			break;
		}
		case e_sstat::uri: {
			switch (len)
			{
			case URI_MAXSIZE:
				throw(statusCodeException(414, "URI Too Long"));
			case 0: {
				if (buffer[i] != '/')
					throw(statusCodeException(400, "Bad Request"));
			}
			}
			switch (ch)
			{
			case '/': {
				string subUri = buffer.substr(i-len, len+1).cppstring();
				if (s.config->locations.find(subUri) != s.config->locations.end())
					s.rules = &(s.config->locations.at(subUri));
				break;
			}
			case '?': {
				if (s.path.empty()) {
					s.path = buffer.substr(i-len, len).cppstring();
					if (s.path[s.path.size()-1] != '/') {
						s.path += '/';
						if (s.config->locations.find(s.path) != s.config->locations.end())
							s.rules = &(s.config->locations.at(s.path));
					}
					len = 0;
					continue;
				}
				break;
			}
			case ' ': {
				if (s.rules == NULL)
					throw(statusCodeException(404, "Not Found"));
				if (s.path.empty()) {
					s.path = buffer.substr(i-len, len).cppstring();
					if (s.path[s.path.size()-1] != '/') {
						s.path += '/';
						if (s.config->locations.find(s.path) != s.config->locations.end())
							s.rules = &(s.config->locations.at(s.path));
					}
				}
				else
					s.query = buffer.substr(i-len+1, len).cppstring();
				s.path.erase(s.path.end()-1);
				s.sstat = e_sstat::httpversion;
				len = 0;
				continue;
			}
			case '-': case '.': case '_': case '~':
        	case ':': case '#': case '[': case ']':
			case '@': case '!': case '$': case '&':
			case '\'': case '(': case ')': case '*':
			case '+': case ',': case ';': case '=':
				break;
			default:
				throw(statusCodeException(400, "Bad Request"));
			}
			++len;
			break;
		}
		case e_sstat::httpversion: {
			switch (len)
			{
			case 7: {
				if (buffer.ncmp("HTTP/1.1", 8, i-len))
					throw(statusCodeException(400, "Bad Request"));
				s.sstat = e_sstat::starterlineNl;
				len = 0;
				continue;
			}
			case 8:
				throw(statusCodeException(400, "Bad Request"));
			}
			++len;
			break;
		}
		case e_sstat::starterlineNl: {
			switch (ch)
			{
			case '\r': {
				if (len != 0)
					throw(statusCodeException(400, "Bad Request"));
				break;
			}
			case '\n': {
				s.sstat = e_sstat::emptyLine;
				return i;
			}
			default:
				throw(statusCodeException(400, "Bad Request"));
			}
			++len;
			break;
		}
		}
	}
	return -1;
}
	// 	case e_sstat::fieldLine: {
	// 		switch (ch)
	// 		{
	// 		case ':': { //test-> : field name ? valid?
	// 			fieldline = buffer.substr(i-xlength.s_field, xlength.s_field).cppstring();
	// 			s.sstat = e_sstat::wssBeforeFieldName;
	// 			break;
	// 		}
	// 		case '-':
	// 		case '_':
	// 			break;
	// 		default: {
	// 			if (!iswalnum(ch))
	// 				throw(statusCodeException(400, "Bad Request"));
	// 		}
	// 		}
	// 		++xlength.s_field;
	// 		++xlength.s_headerfields;
	// 		break;
	// 	}
	// 	case e_sstat::wssBeforeFieldName: {
	// 		switch (ch)
	// 		{
	// 		case '\r': {
	// 			s.sstat = e_sstat::fieldNl;
	// 			break;
	// 		}
	// 		case '\n':
	// 			s.sstat = e_sstat::emptyLine;
	// 		case ' ':
	// 		case '\t':
	// 		case '\f':
	// 		case '\v':
	// 			break;
	// 		default: {
	// 			--i;
	// 			s.sstat = e_sstat::filedName;
	// 			xlength.s_field = 0;
	// 			continue;
	// 		}
	// 		}
	// 		++xlength.s_headerfields;
	// 		break;
	// 	}
	// 	case e_sstat::filedName: {
	// 		switch (ch)
	// 		{
	// 		case '\r': {
	// 			fieldname = buffer.substr(i-xlength.s_field, xlength.s_field).cppstring();
	// 			s.sstat = e_sstat::fieldNl;
	// 			break;
	// 		}
	// 		case '\n': {
	// 			fieldname = buffer.substr(i-xlength.s_field, xlength.s_field).cppstring();
	// 			s.sstat = e_sstat::emptyLine;
	// 		}
	// 		}
	// 		++xlength.s_field;
	// 		++xlength.s_headerfields;
	// 		break ;
	// 	}
	// 	case e_sstat::fieldNl: {
	// 		if (ch != '\n')
	// 			throw(statusCodeException(400, "Bad Request"));
	// 		s.sstat = e_sstat::emptyLine;
	// 		++xlength.s_headerfields;
	// 		break;
	// 	}
	// 	case e_sstat::emptyLine: {
	// 		switch (ch)
	// 		{
	// 		case '\r': {
	// 			if (xlength.s_headersEnd != 0)
	// 				throw(statusCodeException(400, "Bad Request"));
	// 			break;
	// 		}
	// 		case '\n': {
	// 			if (xlength.s_headersEnd > 1)
	// 				throw(statusCodeException(400, "Bad Request"));
	// 			switch (s.method)
	// 			{
	// 				case POST: {
	// 					s.sstat = e_sstat::bodyFormat;
	// 					break;
	// 				}
	// 				default:
	// 					s.sstat = e_sstat::sHeader;
	// 			}
	// 			if (!fieldline.empty())
	// 				s.headers[fieldline] = fieldname;
	// 			break;
	// 		}
	// 		default: {
	// 			if (xlength.s_headersEnd != 0)
	// 				throw(statusCodeException(400, "Bad Request"));
	// 			--i;//goin back to the prev index so i can check if the char is valid in fieldname terms
	// 			s.sstat = e_sstat::fieldLine;
	// 			xlength.s_field = 0;
	// 			if (!fieldline.empty())
	// 				s.headers[fieldline] = fieldname;
	// 			fieldline = "";
	// 			fieldname = "";
	// 			continue;
	// 		}
	// 		}
	// 		++xlength.s_headersEnd;
	// 		++xlength.s_headerfields;
	// 		break;
	// 	}
	// 	case e_sstat::bodyFormat: {
	// 		if (bodyFormat != -1) {
	// 			s.sstat = (bodyFormat == 1) ? s.sstat = e_sstat::contentLengthBased : s.sstat = e_sstat::transferEncodingChunkedBased;
	// 			fd = openFile(s.headers["content-disposition"]);
	// 		}
	// 		else if (s.headers.find("content-type") != s.headers.end() && isMultipartFormData(s.headers["content-type"])) {
	// 			boundary = "--" + s.headers["content-type"].substr(s.headers["content-type"].rfind('=')+1);
	// 			if (s.headers.find("content-length") != s.headers.end()) {
	// 				s.sstat = e_sstat::contentLengthBased;
	// 				length = stoi(s.headers["content-length"]);//it will throw incase of invalid arg
	// 				bodyFormat = 1;
	// 			} else if (s.headers.find("transfer-encoding") != s.headers.end() && s.headers["transfer-encoding"] == "chunked") {
	// 				s.sstat = e_sstat::contentLengthBased;
	// 				bodyFormat = 2;
	// 			}
	// 		}
	// 		else
	// 			throw(statusCodeException(501, "Not Implemented"));
	// 	}
	// 	case e_sstat::contentLengthBased: {
	// 		switch (ch)
	// 		{
	// 			case '\n': {
	// 				if (buffer[i-1] == '\r')
	// 					--xlength.s_bodyLine;
	// 				if (!buffer.ncmp(boundary.c_str(), xlength.s_bodyLine+1, i-xlength.s_bodyLine))
	// 					s.sstat = e_sstat::emptyLine;
	// 				// else if (!buffer.ncmp((boundary+"--").c_str(), xlength.s_bodyLine+1, i-xlength.s_bodyLine))
	// 				// 	//end
	// 				// else
	// 				// 	//write to the file
	// 				xlength.s_bodyLine = 0;
	// 				continue;
	// 			}
	// 		}
	// 		++xlength.s_bodyLine;
	// 		break;
	// 	}
	// 	}
	// 	if (xlength.s_headerfields > HEADER_FIELD_MAXSIZE)
	// 		throw(statusCodeException(431, "Request Header Fields Too Large"));
	// }

