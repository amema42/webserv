#include "../../include/ClientConnection.hpp"
#include <cstring>

ClientConnection::ClientConnection(int fd) : fd(fd), buffer("") {}

ClientConnection::~ClientConnection() {
}

int ClientConnection::getFd() const {
    return fd;
}

std::string &ClientConnection::getBuffer() {
    return buffer;
}


bool ClientConnection::hasCompleteRequest() const {
   // DEBUG std::cout << " ---------------IL BUFFER------------- " <<std::endl << buffer << std::endl << " ---------------FINE BUFFER------------- ";
    size_t headerEnd = buffer.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return false;
        
    std::string headers = buffer.substr(0, headerEnd);
    
    // Controlla se è una richiesta multipart/form-data
    size_t contentTypePos = headers.find("Content-Type:");
    if (contentTypePos != std::string::npos) {
        size_t multipartPos = headers.find("multipart/form-data", contentTypePos);
        if (multipartPos != std::string::npos) {
            size_t boundaryPos = headers.find("boundary=", multipartPos);
            if (boundaryPos != std::string::npos) {
                boundaryPos += 9;
                size_t boundaryEnd = headers.find_first_of("\r\n;", boundaryPos);
                std::string boundary;
                if (boundaryEnd != std::string::npos) {
                    boundary = headers.substr(boundaryPos, boundaryEnd - boundaryPos);
                } else {
                    boundary = headers.substr(boundaryPos);
                }
                std::string endBoundary = "--" + boundary + "--";
                if (buffer.find(endBoundary) != std::string::npos) {
                    std::cout << "Trovata fine del multipart/form-data" << std::endl;
                    return true;
                }
                return false;
            }
        }
    }
    
    // Fallback al metodo Content-Length per richieste non multipart
    size_t contentLengthPos = headers.find("Content-Length:");
    if (contentLengthPos != std::string::npos) {
        size_t valueStart = headers.find_first_not_of(" \t", contentLengthPos + 15);
        if (valueStart != std::string::npos) {
            size_t valueEnd = headers.find_first_of("\r\n", valueStart);
            if (valueEnd != std::string::npos) {
                std::string contentLengthStr = headers.substr(valueStart, valueEnd - valueStart);
                int contentLength = atoi(contentLengthStr.c_str());
                return (buffer.size() >= headerEnd + 4 + contentLength);
            }
        }
    }
    return true;
}

void ClientConnection::removeProcessedRequest(size_t processedLength) {
    if (processedLength < buffer.size()) {
        buffer.erase(0, processedLength);
    } else {
        buffer.clear();
    }
}
