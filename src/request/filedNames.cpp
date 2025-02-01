#include "request.hpp"

bool    Request::validFieldName(string& str) const {
	for (auto& c: str) {
		if (!iswalnum(c) && c != '_' && c != '-')	return false;
		c = tolower(c);
	}
	return true;
}

bool	Request::parseFileds(stringstream& stream) {
	string			line;
	static string	prvsFieldName;

	while(getline(stream, line) && line != " ") {
		string	fieldName;
		string	filedValue;

		if (!headers.empty() && (line[0] == ' ' || line[0] == '\t')) {
			headers[prvsFieldName] += " " + trim(line);
			continue ;
		}

		size_t colonIndex = line.find(':');
		fieldName = line.substr(0, colonIndex);
		if (colonIndex != string::npos && colonIndex+1 < line.size()) {
			filedValue = line.substr(colonIndex+1);
			filedValue = trim(filedValue);
		}
		if (colonIndex == string::npos || !validFieldName(fieldName)) {
			throw(statusCodeException(400, "Bad Request"));
		}
		headers[fieldName] = filedValue;
		prvsFieldName = fieldName;
	}
	if (stream.eof()) {
		remainingBuffer = line;
		return false;
	}
	return true;
}