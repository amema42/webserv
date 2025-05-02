#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>
#include <vector>

class HTTPRequest {
public:
    HTTPRequest();
    ~HTTPRequest();

    // Dati della request
    std::string method;
    std::string uri;
    std::string httpVersion;
    std::map<std::string, std::string> headers; // std::map per gli header, cercare e inserire chiave-valore 
    std::string body;

    // Funcs per il parsing
    void parseRequest(const std::string &rawRequest);
    bool isChunked() const;  // Transfer-Encoding: chunked


};

#endif
