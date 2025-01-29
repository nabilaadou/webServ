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
            throw "expected: `" + key + "` got `" + line + "`";
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

void handleBodyLimit(string& line, int len, keyValue& kv, ifstream& sFile) {
    int i = checkKey("body: ", line);
    kv.bodySize = ft_stoi(trim(line.substr(i)));
}

void handleCgi(string& line, int len, keyValue& kv, ifstream& sFile) {
    if (trim(line) != "[cgi]") {
        throw "expected: `[cgi]` got `" + line + "`";
    }
    while (getline(sFile, line)) {
        if (trim(line) == "[END]") { return ; }
        kv.cgis.push_back(trim(line));
    }
}

void handleError(string& line, int len, keyValue& kv, ifstream& sFile) {
    if (trim(line) != "[errors]") {
        throw "expected: `[errors]` got `" + line + "`";
    }
    while (getline(sFile, line)) {
        if (trim(line) == "[END]") { return ; }
        kv.errorPages.push_back(trim(line));
    }
}

/////////////////////// ROOTS

void handleUrl(string& line, root& kv, ifstream& sFile) {
    int i = checkKey("url: ", line);
    kv.url = trim(line.substr(i));
}

void handleAlias(string& line, root& kv, ifstream& sFile) {
    int i = checkKey("alias: ", line);
    kv.alias = trim(line.substr(i));
}

void handleMethods(string& line, root& kv, ifstream& sFile) {
    int i = checkKey("methods: ", line);
    string tmp;

    line = trim(line.substr(i));
    while (true) {
        i = line.find_first_of(',');
        if (i == string::npos) {
            kv.methods.push_back(trim(line));
            break;
        }
        tmp = line.substr(0, i);
        kv.methods.push_back(trim(tmp));
        line = line.substr(i);
        if (line[0] == ',')
            line = line.substr(1);
    }
    if (kv.methods.size() > 3)
        throw "invalid numbers of methods: " + to_string(kv.methods.size());
    for (int i = 0; i < kv.methods.size(); ++i) {
        if (kv.methods[i] != "GET" && kv.methods[i] != "DELETE" && kv.methods[i] != "POST")
            throw "invalid method: `" + kv.methods[i] + "`";
    }
}

void handleIndex(string& line, root& kv, ifstream& sFile) {
    int i = checkKey("Index: ", line);
    kv.index = trim(line.substr(i));
}

void handleAutoIndex(string& line, root& kv, ifstream& sFile) {
    int i = checkKey("autoIndex: ", line);
    line = trim(line.substr(i));

    if (line == "on")
        kv.autoIndex = true;
    else if (line == "off")
        kv.autoIndex = false;
    else
        throw "invalid autoIndex: `" + line + "`";
}

root handleRoot(ifstream& sFile) {
    void (*farr[])(string& line, root& kv, ifstream& sFile) = {handleUrl, handleAlias, handleMethods, handleIndex, handleAutoIndex};
    string line;
    root kv;
    int i = 0;

    while (getline(sFile, line)) {
        line = trim(line);
        if (line == "[END]") {
            return kv;
        }
        else if (line.empty())
            continue;
        if (i > 4)
            break;
        farr[i](line, kv, sFile);
        i++;
    }
    throw "[END] tag neede";
}

void handlelocs(string& line, int len, keyValue& kv, ifstream& sFile) {
    if (line != "[ROOTS]")
            throw "handlelocs::unknown keywords: `" + line + "`";
    while (getline(sFile, line)) {
        line = trim(line);
        if (line.empty())
            continue;
        if (line == "[root]") {
            kv.roots.push_back(handleRoot(sFile));
        }
        else if (line == "[END]") {
            break ;
        }
        else {
            throw "handlelocs::unknown keywords: `" + line + "`";
        }
    }
}

