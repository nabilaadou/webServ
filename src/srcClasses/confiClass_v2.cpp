#include "confiClass.hpp"

string  trim(const string& str) {
	size_t  start = str.find_first_not_of(" \t\n\r\f\v");
	if (start == string::npos)  return "";
	size_t  end = str.find_last_not_of(" \t\n\r\f\v");
	return str.substr(start, end - start + 1);
}

int checkKey(string key, const string& line) {
    size_t     i;

    for (i = 0; i < key.size(); ++i) {
        if (key[i] != line[i]) {
            throw "invalid: `" + line + "` IN `" + line[i] + "`";
        }
    }
    return (i);
}

int ft_stoi(const std::string &__str) {
    try {
        int res = stoi(__str);
        return (res);
    }
    catch (exception& e) {
        throw "unvalid number: `" + __str + "`";
    }
}

void handlePort(string& line, int len, keyValue& kv, ifstream& sFile) {
    int i = checkKey("port: ", line);
    string tmp;

    line = trim(line.substr(i));
    while (true) {
        i = line.find_first_of(',');
        kv.port.push_back(ft_stoi(trim(line)));
        if (i == string::npos) { break; }
        tmp = line.substr(0, i);
        line = line.substr(i);
        if (line[0] == ',')
            line = line.substr(1);
    }
}

string getCurrentHost(string line) {
    string tmp = trim(line);

    // to do
    // check the ip provided
    return tmp;
}

void handlehost(string& line, int len, keyValue& kv, ifstream& sFile) {
    int i = checkKey("host: ", line);
    string tmp;

    line = trim(line.substr(i));
    while (true) {
        i = line.find_first_of(',');
        if (i == string::npos) {
            kv.host.push_back(getCurrentHost(line));
            break;
        }
        tmp = line.substr(0, i);
        kv.host.push_back(getCurrentHost(tmp));
        line = line.substr(i);
        if (line[0] == ',')
            line = line.substr(1);
    }
}

void handleSerNames(string& line, int len, keyValue& kv, ifstream& sFile) {
    int i = checkKey("serN: ", line);
    string tmp;

    line = trim(line.substr(i));
    while (true) {
        i = line.find_first_of(',');
        if (i == string::npos) {
            kv.serNames.push_back(trim(line));
            break;
        }
        tmp = line.substr(0, i);
        kv.serNames.push_back(trim(tmp));
        line = line.substr(i);
        if (line[0] == ',')
            line = line.substr(1);
    }
}

void handleError(string& line, int len, keyValue& kv, ifstream& sFile) {
    int i = checkKey("[errP]", line);

    while (getline(sFile, line)) {
        if (trim(line) == "[END]") { break; }
        kv.errorPages.push_back(trim(line));
    }
}

void handleBodyLimit(string& line, int len, keyValue& kv, ifstream& sFile) {
    int i = checkKey("body: ", line);
    kv.bodySize = ft_stoi(trim(line.substr(i)));
}

void handlelocs(string& line, int len, keyValue& kv, ifstream& sFile) {

}

