#include "request.hpp"

const string&	Request::getMethod()	const {
	return (method);
}

const string&	Request::getHttpProtocole()	const {
	return (httpVersion);
}

const string	Request::getHeader(const string& header) {
	if (headers.find(header) != headers.end())
		return (headers[header]);
	return "";
}

const string&	Request::getPath()	const {
	return (targetPath);
}

const string&	Request::getQuery()	const {
	return (targetQuery);
}

const t_requestState&	Request::getRequestStatus() const {
	return state;
}