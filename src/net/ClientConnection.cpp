#include "ClientConnection.hpp"
#include <cstring>  // Per std::string::npos

ClientConnection::ClientConnection(int fd) : fd(fd), buffer("") {}

ClientConnection::~ClientConnection() {
    // La chiusura del fd sarà gestita altrove (nel server)
}

int ClientConnection::getFd() const {
    return fd;
}

std::string &ClientConnection::getBuffer() {
    return buffer;
}

bool ClientConnection::hasCompleteRequest() const {
    // Per semplicità, consideriamo completa una richiesta se nel buffer esiste "\r\n\r\n"
    return (buffer.find("\r\n\r\n") != std::string::npos);
}

void ClientConnection::removeProcessedRequest(size_t processedLength) {
    if (processedLength < buffer.size()) {
        buffer.erase(0, processedLength);
    } else {
        buffer.clear();
    }
}
