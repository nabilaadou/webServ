#include "binarystring.hpp"

enum progress {
	NL = 10,
	BR = 9,
};

bstring::bstring() : __string(NULL), stringSize(0) {}

bstring::bstring(const char* str, const size_t size) : stringSize(size) {
	if (str == NULL) {
		__string = NULL;
		stringSize = 0;
	} else {
		__string = new char[stringSize];
		strncpy(__string, str, stringSize);
	}
}

bstring::bstring(const bstring& other) {
	// cerr << "bstring::copy constructer called" << endl;
	if (other.null() == false) {
		stringSize = other.size();
		__string = new char[stringSize];
		strncpy(__string, other.__string, stringSize);
	} else {
		stringSize = 0;
		__string = NULL;
	}
}

bstring::~bstring() {
	// cerr << "bstring::destructor called" << endl;
	if (__string)
		delete []__string;
}

const size_t&	bstring::size() const {
	return stringSize;
}

const string	bstring::cppstring() const {
	std::string s;
	for (int i = 0; i < stringSize; ++i)
		s += __string[i];
	return s;
}

vector<bstring>	bstring::split(const char* seperator) {
	vector<bstring> list;
	size_t			i = 0, pos = 0;
	while(i < stringSize) {
		if (ncmp(seperator, strlen(seperator), i) == 0) {
			list.push_back(substr(pos, i-pos));
			i += strlen(seperator);
			while (ncmp(seperator, strlen(seperator), i) == 0)
				i += strlen(seperator);
			pos = i;
		} else
			++i;
	}
	if (pos < stringSize)
		list.push_back(substr(pos));
	return list;
}

bstring	bstring::substr(unsigned int start, size_t len) {
	size_t	i = 0;

	if (__string == NULL || start >= stringSize)
		return bstring();//default constructer just like returning NULL;
	if (len > stringSize - start || len == std::string::npos)
		len = stringSize - start;
	char substring[len];
	while (i < len && i < stringSize) {
		substring[i] = __string[start + i];
		i++;
	}
	return bstring(substring, len);
}

bool	bstring::getline(bstring& line) {
	char	ch;
	bool	br;
	for (int i = 0; i < stringSize; ++i) {
		ch = __string[i];
		switch (static_cast<int>(ch))
		{
		case NL: {
			line = substr(i);
			return true;
		}
		case BR: {
			if (br == false) {
				br = true;
				break;
			}
		}
		default:
			if (BR)
				cerr << "err" << endl;
				// throw(statusCodeException(400, "Bad Request"));
		}
	}
	line = substr(0);
	return false;
}

bool	bstring::cmp(const char* str) {
	int i = 0;
	while (i < stringSize - 1 && str[i] && __string[i] == str[i])
		++i;
	return str[i] - __string[i];
}

bool	bstring::ncmp(const char* str1, const size_t n, const size_t startpos) {
	if (startpos >= stringSize)
		return true;
	const char* str2 = &(__string[startpos]);
	size_t str2size = stringSize - startpos;
	int i = 0;
	while (i+1 < n && i+1 < str2size && str1[i] && str2[i] == str1[i])
		++i;
	return str1[i] - str2[i];
}


bool	bstring::null() const {
	if (__string == NULL)
		return true;
	return false;
}

const bstring& bstring::operator=(const bstring& other) {
	if (this != &other) {
		delete[] __string;
		stringSize = other.size();
		__string = new char[stringSize];
		strncpy(__string, other.__string, stringSize);
	}
	return *this;
}

const char*	bstring::operator=(const char* newstring) {
	if (__string)
		delete []__string;
	if (newstring == NULL)
		return NULL;
	stringSize = strlen(newstring);
	__string = new char[stringSize];
	strncpy(__string, newstring, stringSize);
	return __string;
}

const char&	bstring::operator[](const int index) const{
	if (index < 0 || index >= stringSize)
		throw(bstringExceptions("index out of range"));
	return __string[index];
}

std::ostream& operator<<(std::ostream &out, const bstring &bs) {
	for (int i = 0; i < bs.size(); ++i)
		out << bs[i];
	return out;
}

// const char*	bstring::operator+=(const char* buff) {

// }