// #include "confiClass.hpp"

// confiClass::confiClass(string _file) { file = _file; }
// confiClass::~confiClass() {}


// string  trim(const string& str) {
// 	size_t  start = str.find_first_not_of(" \t\n\r\f\v");
// 	if (start == string::npos)  return "";

// 	size_t  end = str.find_last_not_of(" \t\n\r\f\v");

// 	return str.substr(start, end - start + 1);
// }

// void confiClass::printKeyValue() {
//     for (size_t i = 0; i < kValue.size(); ++i) {
//         if (i != 0)
//             cout << endl << endl;
//         cout << "------------------SERVER-" << i << "------------------" << endl;
//         cout << "----> Port: " << kValue[i].port << endl;
//         cout << "----> Host: " << kValue[i].host << endl;
//         cout << "----> Server Names:" << endl;
//         for (size_t j = 0; j < kValue[i].serNames.size(); ++j) {
//             cout << "------------------->" << kValue[1].serNames[j] << endl;
//         }
//     }
// }

// int checkKey(string key, const string& line) {
//     size_t     i;

//     for (i = 0; i < key.size(); ++i) {
//         if (key[i] != line[i]) {

//             throw "invalid: " + line + "IN -" + line[i] + "-";
//         }
//     }
//     return (i);
// }

// void confiClass::handlePort(string& line, int len) {
//     int i = checkKey("port: ", line);
//     kValue[len].port = stoi(line.substr(i));
// }

// void confiClass::handlehost(string& line, int len) {
//     unsigned char buf[sizeof(struct in6_addr)];
//     int i = checkKey("host: ", line);
//     line = line.substr(i);

//     int s;

//     s = inet_pton(AF_INET, line.c_str(), buf);
//     if (s == 0)
//         throw "Host is not in presentation format";
//     else if (s < 0)
//         throw "inet_pton failed";
//     kValue[len].host = line;
// }

// void confiClass::handleSerNames(string& line, int len) {
//     size_t i = checkKey("server_names: ", line);
//     string tmp;
//     line = line.substr(i);

    
//     while (true) {
//         i = line.find_first_of(',');
//         if (i == string::npos) {
//             kValue[len].serNames.push_back(trim(line));
//             break;
//         }
//         tmp = line.substr(0, i);
//         line = line.substr(i);
//         kValue[len].serNames.push_back(trim(tmp));
//         if (line[0] == ',')
//             line = line.substr(1);
//     }
// }

// void confiClass::handlelocs(string& line, int len) {
//     (void)len;
//     int i = checkKey("loc: ", line);
//     line = line.substr(i);
    
    
    
//     // cout << "loc: " + line << endl;
// }


// void confiClass::parseFile() {
//     ifstream sFile(file);
//     int len = 0;
//     int i = 0;

//     if (!sFile) {
//         throw "Unable to open file";
//     }
//     string line;
//     while (getline(sFile, line)) {
//         line = trim(line);
//         if (line.empty())
//             continue;
//         if (line == "server:") {
//             i = 0;
//             while (getline(sFile, line))
//             {
//                 if (line[0] != '\t' || line[1] == '\t' || trim(line).empty())
//                     break;

//                 line = trim(line);
//                 if (i == 0)
//                     handlePort(line, len);
//                 else if (i == 1)
//                     handlehost(line, len);
//                 else if (i == 2)
//                     handleSerNames(line, len);
//                 else if (i == 3)
//                     handlelocs(line, len);
//                 i++;
//             }
//         }
//         else if (line != "server:" || !line.empty()) {
//             throw "unknown keywords: `" + line + "`";
//         }
//         len++;
//     }

//     sFile.close();
// }


// int main(int ac, char **av) {
//     if (ac != 2)
//         return 0;
//     try {
//         confiClass confi(av[1]);
//         confi.parseFile();
//         confi.printKeyValue();
//     }
//     catch (const char *s) {
//         cerr << s << endl;
//         return -1;
//     }
//     catch (string s) {
//         cerr << s << endl;
//         return -1;
//     }
//     catch (exception& e) {
//         cerr << e.what() << endl;
//         return -1;
//     }
//     return 0;
// }
