#include "stringManipulation.h"

void	split(const string& str, const char delimiter, vector<string>& parts) {
	int	i = 0, pos = 0;

	while (str[i]) {
		if (str[i] == delimiter) {
			parts.push_back(str.substr(pos, i - pos));
			while (str[i] && str[i] == delimiter)
				++i;
			pos = i;
		}
		else
			++i;
	}
}

inline std::string& trim(std::string& s) {
	s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
	s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
    return s;
}