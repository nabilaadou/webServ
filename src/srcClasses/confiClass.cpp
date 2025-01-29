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
        // cout << line << endl;
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
        cout << "---------> Ports:";
        for (size_t j = 0; j < kValue[i].port.size(); ++j) {
            cout << " " << kValue[i].port[j];
            if (j + 1 < kValue[i].port.size())
                cout << ",";
        }
        cout << endl << "---------> hosts:";
        for (size_t j = 0; j < kValue[i].host.size(); ++j) {
            cout << " " << kValue[i].host[j];
            if (j + 1 < kValue[i].host.size())
                cout << ",";
        }
        cout << endl << "---------> Server Names:";
        for (size_t j = 0; j < kValue[i].serNames.size(); ++j) {
            cout << " " << kValue[i].serNames[j];
            if (j + 1 < kValue[i].serNames.size())
                cout << ",";
        }
        cout << endl << "---------> Body Size:" << kValue[i].bodySize << "M" << endl;
        cout << "---------> Error Pages:";
        for (size_t j = 0; j < kValue[i].errorPages.size(); ++j) {
            cout << " " << kValue[i].errorPages[j];
            if (j + 1 < kValue[i].errorPages.size())
                cout << ",";
        }
        cout << endl << "---------> ROOTS:" << endl;
        for (size_t j = 0; j < kValue[i].roots.size(); ++j) {
            cout << "------------------> ROOT:" << endl;
            cout << "---------------------------> url:       " << kValue[i].roots[j].url << endl;
            cout << "---------------------------> alias:     " << kValue[i].roots[j].alias << endl;
            cout << "---------------------------> Methods:   ";
            for (size_t k = 0; k < kValue[i].roots[j].methods.size(); ++k) {
                cout << kValue[i].roots[j].methods[k];
                if (k + 1 < kValue[i].roots[j].methods.size())
                    cout << ", ";
            }
            cout << endl;
            cout << "---------------------------> index:     " << kValue[i].roots[j].index << endl;
            cout << "---------------------------> autoIndex: " << (kValue[i].roots[j].autoIndex ? "True" : "False") << endl;
        }
    }
}

