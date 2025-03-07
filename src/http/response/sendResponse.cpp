#include "httpSession.hpp"

string httpSession::Response::getSupportedeExtensions(const string& key) {
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
        ext[".odt"]   = "application/vnd.oasis->opendocument.text";
        ext[".ods"]   = "application/vnd.oasis->opendocument.spreadsheet";
        ext[".odp"]   = "application/vnd.oasis->opendocument.presentation";
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

string	httpSession::Response::contentTypeHeader() const {
    //by default application/octet-stream
	size_t pos = s.path.rfind(".");
	// if (pos == string::npos)
	// 	throw(statusCodeException(501, "Not Implemented"));
	string ext = s.path.substr(pos);
	string contentTypeValue = getSupportedeExtensions(ext);
	// if (contentTypeValue.empty())
	// 	throw(statusCodeException(501, "Not Implemented"));
	return ("Content-Type: " + contentTypeValue + "\r\n");
}

void	httpSession::Response::sendHeader(const int clientFd) {
	string header;

	header += "HTTP/1.1" + to_string(s.statusCode) + " " + s.codeMeaning + "\r\n";
    if (s.method == POST)
        header += "Content-Length: 0\r\n";
    else {
        header += contentTypeHeader();
        header += "Transfer-Encoding: chunked\r\n";
    }
	header += "Connection: keep-alive\r\n";//incase clinet has decided to close the connection i ll have to close it too
	header += "Server: bngn/0.1\r\n";
	header += "\r\n";
	if (send(clientFd, header.c_str(), header.size(), MSG_DONTWAIT) <= 0) {
		perror("send failed(sendResponse.cpp 24)");
		s.sstat = CCLOSEDCON;
		return ;
	}
}

void	httpSession::Response::sendBody(const int clientFd) {
	char buff[BUFFER_SIZE];

	if (contentFd == -1) {
		if ((contentFd = open(s.path.c_str(), O_RDONLY, 0644)) == -1) {//read about permissions again you FORGOT (0644???)
			perror("open failed(sendresponse.cpp 37)");
			throw(statusCodeException(500, "Internal Server Error"));
		}
	}
	int sizeRead;
	if((sizeRead = read(contentFd, buff, BUFFER_SIZE)) < 0) {
		perror("read failed(sendresponse.cpp 43)");
		throw(statusCodeException(500, "Internal Server Error"));
	}
	if (sizeRead > 0) {
        bstring         chunkedResponse;
		ostringstream   chunkSize;

		chunkSize << hex << sizeRead << "\r\n";
        chunkedResponse += chunkSize.str().c_str();
        bstring tmpBuffer(buff, sizeRead);
        chunkedResponse += tmpBuffer;
        chunkedResponse += "\r\n";
        if (send(clientFd, chunkedResponse.c_str(), chunkedResponse.size(), MSG_DONTWAIT) <= 0) {
            perror("send failed(sendResponse.cpp 50)");
			s.sstat = CCLOSEDCON;
			return ;
        }
	} else {
		if (send(clientFd, "0\r\n\r\n", 5, MSG_DONTWAIT) <= 0) {
			perror("end failed(sendResponse.cpp 56)");
			s.sstat = CCLOSEDCON;
			return ;
		}
		s.sstat = done;
		close(contentFd);
	}
}

static bstring    tweakAndCheckHeaders(map<string, string>& headers) {
    bstring bheaders;

    if (headers.find("content-type") == headers.end())
        headers["content-type"] = "text/plain";
    if (headers.find("content-length") != headers.end())
        headers.erase("content-length");
    if (headers.find("transfer-encoding") != headers.end())
        headers.erase("transfer-encoding");
    headers["transfer-encoding"] = "chunked";
    if (headers.find("connection") == headers.end()) {
        headers["connection"] = "close";
    } else {
        if (headers["connection"] != "close" || headers["connection"] != "keep-alive")
            headers["connection"] = "close";
    }
    for (map<string, string>::iterator it = headers.begin(); it != headers.end(); ++it) {
        bheaders += (it->first + ": " + it->second + "\r\n").c_str();
    }
    bheaders += "\r\n";
    return bheaders;
}

void    httpSession::Response::sendCgiOutput(const int clientFd) {
    char    buff[BUFFER_SIZE];
    int     byteRead = 0;
    int     status;

    if (!s.cgiBody.empty()) {
        int     byteWrite;

        if ((byteWrite = write(s.cgi->wFd(), s.cgiBody.c_str(), s.cgiBody.size())) < 0) {
            perror("write failed(sendResponse.cpp 185)");
            s.sstat = CCLOSEDCON;
            return;
        }
        cerr << "byte write: " << byteWrite << endl;
        s.cgiBody.erase(0, byteWrite);
        if (s.cgiBody.empty())
            cerr << "done writing the vody to the script" << endl;
    }
    else if ((byteRead = read(s.cgi->rFd(), buff, BUFFER_SIZE)) < 0) {
        perror("read failed(sendResponse.cpp 152)");
        s.sstat = CCLOSEDCON;
        return;
    }
    bstring bbuffer(buff, byteRead);
    if (byteRead > 0) {
        bstring         chunkedResponse;
		ostringstream   chunkSize;
    
        if (cgiHeadersParsed == false) {
            map<string, string>	cgiHeaders;
            size_t  bodyStartPos = 0;

            s.sstat = e_sstat::emptyline;
            try {
                if ((bodyStartPos = s.parseFields(bbuffer, 0, cgiHeaders)) < 0) { // i might use a buffer here in case of incomplete fields
                    perror("write failed(sendResponse.cpp 143)");
		            s.sstat = CCLOSEDCON;
                    return;
                }
            } catch (...) {
                s.sstat = CCLOSEDCON;
                return;
            }
            s.sstat = e_sstat::sBody;
            chunkedResponse += ("HTTP/1.1 " + to_string(s.statusCode) + " " + s.codeMeaning + "\r\n").c_str();
            chunkedResponse += tweakAndCheckHeaders(cgiHeaders);
            bbuffer = bbuffer.substr(bodyStartPos);
            cgiHeadersParsed = true;
        }
		chunkSize << hex << bbuffer.size() << "\r\n";
        chunkedResponse += chunkSize.str().c_str();
        chunkedResponse += bbuffer;
        chunkedResponse += "\r\n";
        cerr << "cgi response ->>>>>>" << endl;
        cerr << chunkedResponse << endl;
        cerr << "-------------" << endl;
        if (send(clientFd, chunkedResponse.c_str(), chunkedResponse.size(), MSG_DONTWAIT) <= 0) {
            perror("send failed(sendResponse.cpp 50)");
			s.sstat = CCLOSEDCON;
			return ;
        }
    } else if (waitpid(s.cgi->ppid(), &status, WNOHANG) > 0) {
        if (send(clientFd, "0\r\n\r\n", 5, MSG_DONTWAIT) <= 0) {
			perror("write failed(sendResponse.cpp 56)");
			s.sstat = CCLOSEDCON;
			return ;
		}
        s.sstat = done;
		close(s.cgi->rFd());
		close(s.cgi->wFd());
    }
}