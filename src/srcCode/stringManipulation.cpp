#include "webServ.hpp"

string toString(const int& nbr) {
    ostringstream oss;

    oss << nbr;
    return (oss.str());
}

string getSupportedeExtensions(const string& key) {
    static map<string, string> ext;
    
    if (ext.empty()) {
        ext[".7z"]    = "application/x-7z-compressed\r\n";
        ext[".avi"]   = "video/x-msvideo\r\n";
        ext[".bat"]   = "application/x-msdownload\r\n";
        ext[".bin"]   = "application/octet-stream\r\n";
        ext[".bmp"]   = "image/bmp\r\n";
        ext[".css"]   = "text/css\r\n";
        ext[".csv"]   = "text/csv\r\n";
        ext[".doc"]   = "application/msword\r\n";
        ext[".docx"]  = "application/vnd.openxmlformats-officedocument.wordprocessingml.document\r\n";
        ext[".dll"]   = "application/octet-stream\r\n";
        ext[".exe"]   = "application/octet-stream\r\n";
        ext[".eot"]   = "application/vnd.ms-fontobject\r\n";
        ext[".gif"]   = "image/gif\r\n";
        ext[".gz"]    = "application/gzip\r\n";
        ext[".html"]  = "text/html\r\n";
        ext[".ico"]   = "image/x-icon\r\n";
        ext[".iso"]   = "application/octet-stream\r\n";
        ext[".js"]    = "text/javascript\r\n";
        ext[".jpg"]   = "images/jpeg\r\n";
        ext[".jpeg"]  = "image/jpeg\r\n";
        ext[".json"]  = "application/json\r\n";
        ext[".java"]  = "text/x-java-source\r\n";
        ext[".mjs"]   = "text/javascript\r\n";
        ext[".mp3"]   = "audio/mpeg\r\n";
        ext[".mp4"]   = "video/mp4\r\n";
        ext[".mov"]   = "video/quicktime\r\n";
        ext[".mkv"]   = "video/x-matroska\r\n";
        ext[".ogg"]   = "audio/ogg\r\n";
        ext[".odt"]   = "application/vnd.oasis.opendocument.text\r\n";
        ext[".ods"]   = "application/vnd.oasis.opendocument.spreadsheet\r\n";
        ext[".odp"]   = "application/vnd.oasis.opendocument.presentation\r\n";
        ext[".otf"]   = "font/otf\r\n";
        ext[".png"]   = "images/png\r\n";
        ext[".pdf"]   = "application/pdf\r\n";
        ext[".ppt"]   = "application/vnd.ms-powerpoint\r\n";
        ext[".pptx"]  = "application/vnd.openxmlformats-officedocument.presentationml.presentation\r\n";
        ext[".php"]   = "application/x-httpd-php\r\n";
        ext[".py"]    = "text/x-python\r\n";
        ext[".rar"]   = "application/x-rar-compressed\r\n";
        ext[".rtf"]   = "application/rtf\r\n";
        ext[".svg"]   = "image/svg+xml\r\n";
        ext[".sh"]    = "application/x-sh\r\n";
        ext[".sfnt"]  = "font/sfnt\r\n";
        ext[".txt"]   = "text/plain\r\n";
        ext[".tiff"]  = "image/tiff\r\n";
        ext[".tar"]   = "application/x-tar\r\n";
        ext[".ttf"]   = "font/ttf\r\n";
        ext[".webp"]  = "image/webp\r\n";
        ext[".wav"]   = "audio/wav\r\n";
        ext[".webm"]  = "video/webm\r\n";
        ext[".woff"]  = "font/woff\r\n";
        ext[".woff2"] = "font/woff2\r\n";
        ext[".xml"]   = "application/xml\r\n";
        ext[".xls"]   = "application/vnd.ms-excel\r\n";
        ext[".xlsx"]  = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet\r\n";
        ext[".zip"]   = "application/zip\r\n";
    }
    else {
        if (ext.find(key) != ext.end())
            return ext[key];
    }
    return "";
}

string  trim(const string& str) {
	size_t  start = str.find_first_not_of(" \t\n\r\f\v");
	if (start == string::npos)  return "";

	size_t  end = str.find_last_not_of(" \t\n\r\f\v");

	return str.substr(start, end - start + 1);
}
