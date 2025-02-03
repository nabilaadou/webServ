#include "request.hpp"

const string&	Request::Method()	const {
	return (method);
}

const string&	Request::HttpProtocole()	const {
	return (httpVersion);
}

const string	Request::Header(const string& header) {
	if (headers.find(header) != headers.end())
		return (headers[header]);
	return "";
}

const string&	Request::Path()	const {
	return (targetPath);
}

const string&	Request::Query()	const {
	return (targetQuery);
}

const t_requestState&	Request::RequestStatus() const {
	return state;
}