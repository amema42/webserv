#include "HTTPResponse.hpp"
#include <sstream>
#include <string>

HTTPResponse::HTTPResponse() : httpVersion("HTTP/1.1"), statusCode(200), statusMessage("OK") {
    // init settings: e.s. aggiungere "header standard" (Content-Type: text/html)
    headers["Content-Type"] = "text/html";
}

HTTPResponse::~HTTPResponse() {
    // If/When neccessary, 2 free resources
}

void HTTPResponse::setStatus(int code, const std::string &message) {
    statusCode = code;
    statusMessage = message;
}

void HTTPResponse::setHeader(const std::string &key, const std::string &value) {
    headers[key] = value;
}

std::string HTTPResponse::toString() const {
    std::ostringstream oss;
    // Ccompose/ or Build Status-Line
    oss << httpVersion << " " << statusCode << " " << statusMessage << "\r\n";
    // Add headers
    for (std::map<std::string, std::string>::const_iterator it = headers.begin();
         it != headers.end(); ++it) {
        oss << it->first << ": " << it->second << "\r\n";
    }
    // header terinator with "\r\n"
    oss << "\r\n";
    // Add body
    oss << body;
    return oss.str();
}

std::map<std::string, std::string>& HTTPResponse::getHeaders(){
	return this->headers;
}
