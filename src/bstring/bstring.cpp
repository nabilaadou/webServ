#include "binarystring.hpp"

bstring::bstring() : __string(NULL), stringsize(0) {}

bstring::bstring(const char* str, const size_t size) : stringsize(size) {
	if (str == NULL) {
		__string = NULL;
		stringsize = 0;
	} else {
		__string = new char[stringsize];
		strncpy(__string, str, stringsize);
	}
}

bstring::bstring(const bstring& other) {
	// cerr << "bstring::copy constructer called" << endl;
	if (other.null() == false) {
		stringsize = other.size();
		__string = new char[stringsize];
		strncpy(__string, other.__string, stringsize);
	} else {
		stringsize = 0;
		__string = NULL;
	}
}

bstring::~bstring() {
	// cerr << "bstring::destructor called" << endl;
	if (__string)
		delete []__string;
}

const size_t&	bstring::size() const {
	return stringsize;
}

const string	bstring::cppstring() const {
	std::string s;
	for (int i = 0; i < stringsize; ++i)
		s += __string[i];
	return s;
}

vector<bstring>	bstring::split(const char* seperator) {
	vector<bstring> list;
	size_t			i = 0, pos = 0;
	while(i < stringsize) {
		if (ncmp(seperator, strlen(seperator), i) == 0) {
			list.push_back(substr(pos, i-pos));
			i += strlen(seperator);
			while (ncmp(seperator, strlen(seperator), i) == 0)
				i += strlen(seperator);
			pos = i;
		} else
			++i;
	}
	if (pos < stringsize)
		list.push_back(substr(pos));
	return list;
}

bstring	bstring::substr(unsigned int start, size_t len) {
	size_t	i = 0;

	if (__string == NULL || start >= stringsize)
		return bstring();//default constructer just like returning NULL;
	if (len > stringsize - start || len == std::string::npos)
		len = stringsize - start;
	char substring[len];
	while (i < len && i < stringsize) {
		substring[i] = __string[start + i];
		i++;
	}
	return bstring(substring, len);
}

bool	bstring::getline(bstring& line) {
	char	ch;
	bool	br = false;
	for (int i = 0; i < stringsize; ++i) {
		ch = __string[i];
		switch (static_cast<int>(ch))
		{
		case 10: {
			line = substr(0, i);
			erase(0, i+1);
			return true;
		}
		case 9: {
			if (br == false) {
				br = true;
				break;
			}
		}
		default:
			if (br)
				cerr << "err" << endl;
				// throw(statusCodeException(400, "Bad Request"));
		}
	}
	line = substr(0);
	erase(0, std::string::npos);
	return false;
}

void	bstring::erase(const size_t start, size_t n) {
	if (start >= stringsize)
		return ;
	else if (n >= stringsize) {
		n = stringsize - start;
	}
	if (start+n >= stringsize) {
		delete []__string;
		__string = NULL;
		stringsize = 0;
		return ;
	}
	char* newstring = new char[stringsize-n];
	size_t i = 0, newstringindex = 0;
	while (i < start) {
		newstring[newstringindex] = __string[i];
		++i;
		++newstringindex;
	}
	i = start + n;
	while (i < stringsize) {
		newstring[newstringindex] = __string[i];
		++i;
		++newstringindex;
	}
	delete[] __string;
	__string = newstring;
	stringsize = stringsize - n;
}

size_t	bstring::find(const char* needle, const size_t startpos) {
	if (startpos >= stringsize)
		return std::string::npos;
	for (int i = startpos; i < stringsize; ++i) {
		if (ncmp(needle, strlen(needle), i) == 0)
			return i;
	}
	return std::string::npos;
}

bool	bstring::cmp(const char* str) {
	size_t	i = 0;
	if (strlen(str) != stringsize)
		return true;
	while(i+1 < stringsize && str[i] && __string[i] == str[i])
		++i;
	return __string[i] - str[i];
}

bool	bstring::ncmp(const char* str1, const size_t n, const size_t startpos) {
	if (startpos >= stringsize)
		return true;
	const char* str2 = &(__string[startpos]);
	size_t str2size = stringsize - startpos;
	if (n > str2size)
		return true;
	int i = 0;
	while (i+1 < n && i+1 < str2size && str1[i] && str2[i] == str1[i])
		++i;
	return str1[i] - str2[i];
}


bool	bstring::null() const {
	return __string == NULL;
}

const bstring& bstring::operator=(const bstring& other) {
	if (this != &other) {
		delete[] __string;
		stringsize = other.size();
		__string = new char[stringsize];
		strncpy(__string, other.__string, stringsize);
	}
	return *this;
}

const char*	bstring::operator=(const char* newstring) {
	if (__string)
		delete []__string;
	if (newstring == NULL)
		return NULL;
	stringsize = strlen(newstring);
	__string = new char[stringsize];
	strncpy(__string, newstring, stringsize);
	return __string;
}

const char&	bstring::operator[](const int index) const{
	if (index < 0 || index >= stringsize)
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