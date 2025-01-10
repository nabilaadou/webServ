#include "webServ.hpp"

string toString(const int& nbr) {
    std::ostringstream oss;

    oss << nbr;
    return (oss.str());
}

u_map webServ::getSupportedeExtensions() {
    u_map ext;

    ext[".html"] = "Content-Type: text/html\r\n";
    ext[".htm"]  = "Content-Type: text/html\r\n";
    ext[".css"]  = "Content-Type: text/css\r\n";
    ext[".js"]   = "Content-Type: text/javascript\r\n";
    ext[".mjs"]  = "Content-Type: text/javascript\r\n";
    ext[".csv"]  = "Content-Type: text/csv\r\n";
    ext[".png"]  = "Content-Type: images/png\r\n";
    ext[".txt"]  = "Content-Type: text/plan\r\n";
    return ext;
}

string webServ::getFile(string str) {
    std::istringstream  iss(str);
    string              path;
    string              ext;

    iss >> path;
    iss >> path;
    if (path == "/") {
        return ("./index.html");
    }
    ext = path.substr(path.find_last_of("."));
    fileType = extensions[ext];
    return ("." + path);
}

