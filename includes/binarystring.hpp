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
	size_t	stringsize;
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
	~bstring();
	const size_t&	size() const;
	const string	cppstring() const;
	const char*		c_str() const;
	bstring			substr(size_t start, size_t len=std::string::npos);
	vector<bstring>	split(const char* seperator = " ");
	bool			getheaderline(bstring&);
	bool			getline(bstring&);
	void			erase(const size_t start, size_t n);
	size_t			find(const char*, const size_t startpos = 0);
	// size_t			rfind();
	bool			cmp(const char* str);
	bool			ncmp(const char* str1, const size_t n, const size_t startpos = 0);
	bool			null() const;
	const char* 	operator=(const char*);
	const bstring& 	operator=(const bstring&);
	const char& 	operator[](const int) const;
	const bstring&	operator+=(const bstring&);
	const char*		operator+=(const char*);
};

std::ostream& operator<<(std::ostream &out, const bstring &fixed);