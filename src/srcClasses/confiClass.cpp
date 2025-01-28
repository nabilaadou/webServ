#include "confiClass.hpp"

confiClass::confiClass(string _file) {
    file = _file;
}
confiClass::~confiClass() {}

keyValue confiClass::handleServer(ifstream& sFile) {
    void (*farr[])(string& line, int len, keyValue& kv, ifstream& sFile) = {handlePort, handlehost, handleSerNames, handleBodyLimit, handleError, handlelocs};
    string line;
    keyValue kv;
    int i = 0;

    while (getline(sFile, line)) {
        line = trim(line);
        if (line == "[END]")
            return kv;
        else if (line.empty())
            continue;
        if (i > 5)
            break;
        farr[i](line, i, kv, sFile);
        i++;
    }
    throw "[END] tag neede";
}

void confiClass::parseFile() {
    ifstream sFile(file);
    string line;
    int i = 0;
    keyValue kv;

    if (!sFile) {
        throw "Unable to open file";
    }
    while (getline(sFile, line)) {
        line = trim(line);
        if (line.empty())
            continue;
        if (trim(line) == "[server]") {
            kv = handleServer(sFile);
        }
        else {
            throw "parseFile::unknown keywords: `" + line + "`";
        }
        kValue[i++] = kv;
    }
    sFile.close();
}


int main(int ac, char **av) {
    if (ac != 2) {
        cout << "invalid numbers of argument!" << endl;
        return -1;
    }
    try {
        confiClass confi(av[1]);
        confi.parseFile();
        confi.printKeyValue();
    }
    catch (const char *s) {
        cerr << s << endl;
        return -1;
    }
    catch (string s) {
        cerr << s << endl;
        return -1;
    }
    catch (exception& e) {
        cerr << e.what() << endl;
        return -1;
    }
    return 0;
}


















void confiClass::printKeyValue() {
    for (size_t i = 0; i < kValue.size(); ++i) {
        if (i != 0)
            cout << "\n\n                              ------------------------------------\n\n" << endl;
        cout << "------------------SERVER-" << i << "------------------" << endl;
        cout << "----------> Ports:" << endl;
        for (size_t j = 0; j < kValue[i].port.size(); ++j) {
            cout << "-------------------> " << kValue[i].port[j] << endl;
        }
        cout << "----------> hosts:" << endl;
        for (size_t j = 0; j < kValue[i].host.size(); ++j) {
            cout << "-------------------> " << kValue[i].host[j] << endl;
        }
        cout << "----------> Server Names:" << endl;
        for (size_t j = 0; j < kValue[i].serNames.size(); ++j) {
            cout << "-------------------> " << kValue[i].serNames[j] << endl;
        }
        cout << "----------> Body Size:" << endl;
        cout << "-------------------> " << kValue[i].bodySize << "M" << endl;
        cout << "----------> Error Pages:" << endl;
        for (size_t j = 0; j < kValue[i].errorPages.size(); ++j) {
            cout << "-------------------> " << kValue[i].errorPages[j] << endl;
        }
    }
}

