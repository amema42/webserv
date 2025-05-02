#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>

class HTTPResponse {
public:
    HTTPResponse();
    ~HTTPResponse();

    // Response status 
    std::string httpVersion;                    // example: "HTTP/1.1"
    int statusCode;                             // example: 200, 404, ecc.
    std::string statusMessage;                  // example: "OK", "Not Found", ecc.
    std::map<std::string, std::string> headers; // Header 
    std::string body;                           // Body

    // Metodo per "comporre" la risposta finale (in formato str)
    std::string toString() const;

    // Metodo helper per impostare i valori standard (es. "200 OK")
    void setStatus(int code, const std::string &message);

    // Metodo helper per (1) aggiungere o (2) modificare un header
    void setHeader(const std::string &key, const std::string &value);

	std::map<std::string, std::string>& getHeaders();
};

#endif
