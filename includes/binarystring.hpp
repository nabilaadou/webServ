#pragma once

#include <unistd.h>
#include <string>
#include <string.h>
#include "statusCodeException.hpp"
#include <vector>
#include <iostream>
#include <exception>

class bstring {
protected:
	size_t	stringSize;
	char*	__string;
public:
	class bstringExceptions : std::exception {
	private:
		const std::string msg;
	public:
		virtual const char* what() const throw();
		virtual ~bstringExceptions();
		bstringExceptions(std::string);
	};

	bstring();
	bstring(const char* str ,const size_t size);
	bstring(const bstring&);
	// bstring(const bstring&&) throw();
	~bstring();
	const size_t&	size() const;
	const char*		c_str() const;
	bstring			substr(unsigned int start, size_t len=std::string::npos);
	vector<bstring>	split(const char seperator = ' ');
	bool			null() const;
	bool			getline(bstring&);
	const char* 	operator=(const char*);
	const bstring& 	operator=(const bstring&);
	const char& 	operator[](const int) const;
	// const char* operator+=(const char*);
};