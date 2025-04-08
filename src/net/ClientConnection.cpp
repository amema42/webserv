#include "../../include/ClientConnection.hpp"
#include <cstring>  // Per std::string::npos

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
    size_t headerEnd = buffer.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return false;
    std::string headers = buffer.substr(0, headerEnd); 
    size_t contentLengthPos = headers.find("Content-Length:");
    if (contentLengthPos != std::string::npos) {
        // getting value of content length
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
