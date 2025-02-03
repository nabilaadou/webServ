#include <string>
#include <vector>
#include <map>

using namespace std;

string getSupportedeExtensions(const string& key) {
    static map<string, string> ext;
    
    if (ext.empty()) {
        ext[".7z"]    = "application/x-7z-compressed";
        ext[".avi"]   = "video/x-msvideo";
        ext[".bat"]   = "application/x-msdownload";
        ext[".bin"]   = "application/octet-stream";
        ext[".bmp"]   = "image/bmp";
        ext[".css"]   = "text/css";
        ext[".csv"]   = "text/csv";
        ext[".doc"]   = "application/msword";
        ext[".docx"]  = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
        ext[".dll"]   = "application/octet-stream";
        ext[".exe"]   = "application/octet-stream";
        ext[".eot"]   = "application/vnd.ms-fontobject";
        ext[".gif"]   = "image/gif";
        ext[".gz"]    = "application/gzip";
        ext[".html"]  = "text/html";
        ext[".ico"]   = "image/x-icon";
        ext[".iso"]   = "application/octet-stream";
        ext[".js"]    = "text/javascript";
        ext[".jpg"]   = "images/jpeg";
        ext[".jpeg"]  = "image/jpeg";
        ext[".json"]  = "application/json";
        ext[".java"]  = "text/x-java-source";
        ext[".mjs"]   = "text/javascript";
        ext[".mp3"]   = "audio/mpeg";
        ext[".mp4"]   = "video/mp4";
        ext[".mov"]   = "video/quicktime";
        ext[".mkv"]   = "video/x-matroska";
        ext[".ogg"]   = "audio/ogg";
        ext[".odt"]   = "application/vnd.oasis.opendocument.text";
        ext[".ods"]   = "application/vnd.oasis.opendocument.spreadsheet";
        ext[".odp"]   = "application/vnd.oasis.opendocument.presentation";
        ext[".otf"]   = "font/otf";
        ext[".png"]   = "images/png";
        ext[".pdf"]   = "application/pdf";
        ext[".ppt"]   = "application/vnd.ms-powerpoint";
        ext[".pptx"]  = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
        ext[".php"]   = "application/x-httpd-php";
        ext[".py"]    = "text/x-python";
        ext[".rar"]   = "application/x-rar-compressed";
        ext[".rtf"]   = "application/rtf";
        ext[".svg"]   = "image/svg+xml";
        ext[".sh"]    = "application/x-sh";
        ext[".sfnt"]  = "font/sfnt";
        ext[".txt"]   = "text/plain";
        ext[".tiff"]  = "image/tiff";
        ext[".tar"]   = "application/x-tar";
        ext[".ttf"]   = "font/ttf";
        ext[".webp"]  = "image/webp";
        ext[".wav"]   = "audio/wav";
        ext[".webm"]  = "video/webm";
        ext[".woff"]  = "font/woff";
        ext[".woff2"] = "font/woff2";
        ext[".xml"]   = "application/xml";
        ext[".xls"]   = "application/vnd.ms-excel";
        ext[".xlsx"]  = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
        ext[".zip"]   = "application/zip";
    }
    if (ext.find(key) != ext.end())
        return ext[key];
    return "";
}

string  trim(const string& str) {
	size_t  start = str.find_first_not_of(" \t\n\r\f\v");
	if (start == string::npos)  return "";

	size_t  end = str.find_last_not_of(" \t\n\r\f\v");

	return str.substr(start, end - start + 1);
}
