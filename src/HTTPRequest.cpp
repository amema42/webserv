#include "HTTPRequest.hpp"
#include <sstream>
#include <algorithm>

HTTPRequest::HTTPRequest() {
}

HTTPRequest::~HTTPRequest() {
}

bool HTTPRequest::isChunked() const {
    std::map<std::string, std::string>::const_iterator it = headers.find("Transfer-Encoding");
    if (it != headers.end()) {
        std::string value = it->second;
        // Convertiamo in minuscolo per evitare problemi di case-sensitive
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        return (value.find("chunked") != std::string::npos);
    }
    return false;
}
// --- rawRequest: stringa contenente la richiesta HTTP ( *presumibilmente* ) ---
void HTTPRequest::parseRequest(const std::string &rawRequest) {
    // Cerca la fine degli header: "\r\n\r\n"
    std::size_t pos = rawRequest.find("\r\n\r\n");
    if (pos == std::string::npos) {
        // Qui, in un'applicazione reale, potresti decidere di attendere altri dati.
        // Per ora, usciamo dalla funzione o segnaliamo un errore.
        return;
    }
    // La parte degli header termina in pos + 4
    std::string headerPart = rawRequest.substr(0, pos);
    // Il body (se presente) inizia dopo "\r\n\r\n"
    std::string bodyPart = rawRequest.substr(pos + 4);

    // --- Parsing della Request Line ---
    // La prima linea degli headerPart è la request line
    std::istringstream headerStream(headerPart);
    std::string requestLine;
    std::getline(headerStream, requestLine);
    if (!requestLine.empty() && requestLine[requestLine.size() - 1] == '\r')
        requestLine.erase(requestLine.size() - 1);

    std::istringstream requestLineStream(requestLine);
    requestLineStream >> method >> uri >> httpVersion;

    // --- Parsing degli Header ---
    std::string line;
    while (std::getline(headerStream, line)) {
        if (line.empty() || line == "\r")
            continue;
        // Rimuovi il \r finale, se presente
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        // Cerca il separatore ":"
        std::size_t colonPos = line.find(":");
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            // Rimuovi spazi iniziali e finali
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            headers[key] = value;
        }
    }

    // --- Body ---
    // Se è presente un header Content-Length, assicuriamoci di avere quel numero di byte
    std::map<std::string, std::string>::iterator it = headers.find("Content-Length");
    if (it != headers.end()) {
        int contentLength = std::atoi(it->second.c_str());
        // Se il body letto è inferiore a Content-Length, in una implementazione reale, 
        // attendi ulteriori dati. Per ora, assumiamo che rawRequest contenga tutto.
        if (static_cast<int>(bodyPart.size()) >= contentLength)
            body = bodyPart.substr(0, contentLength);
        else
            body = bodyPart;  // oppure gestire l'errore
    } else if (isChunked()) {
        // Se la richiesta è chunked, bisogna implementare il parsing dei chunk
        // Per ora, potresti salvare l'intero body e implementare il parsing chunked in seguito
        body = bodyPart;
        // In seguito, decodificherai il body chunked in una funzione apposita
    } else {
        // Altrimenti, assegna il body (se presente) così com'è
        body = bodyPart;
    }
}
