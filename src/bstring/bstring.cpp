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
	if (other.null() == false) {
		stringSize = other.size();
		__string = new char[stringSize];
		strncpy(__string, other.c_str(), stringSize);
	} else {
		stringSize = 0;
		__string = NULL;
	}
}

// bstring::bstring(const bstring&& other) throw(){
// 	cerr << "bstring::move constructer called" << endl;
// 	stringSize = other.stringSize;
// 	__string = other.__string;
// }

bstring::~bstring() {
	cerr << "bstring::destructor called" << endl;
	delete []__string;
}

const size_t&	bstring::size() const {
	return stringSize;
}

const char*	bstring::c_str() const {
	return __string;
}

vector<bstring>	bstring::split(const char seperator) {
	vector<bstring> list;
	size_t			i = 0, pos = 0;
	while(i < stringSize) {
		if (__string[i] == seperator) {
			cerr << "bef" << endl;
			list.push_back(substr(pos, i-pos));
			cerr << "aft" << endl;
			while (i < stringSize && __string[i] == seperator)
				++i;
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

bool	bstring::null() const {
	if (__string == NULL)
		return true;
	return false;
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

const bstring& bstring::operator=(const bstring& other) {
	if (this != &other) {
		delete[] __string;
		stringSize = other.size();
		__string = new char[stringSize];
		strncpy(__string, other.c_str(), stringSize);
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

// const char*	bstring::operator+=(const char* buff) {

// }