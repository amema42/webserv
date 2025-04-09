#ifndef CLIENTCONNECTION_HPP
#define CLIENTCONNECTION_HPP

#include "webServ.hpp"

class ClientConnection {
public:
    ClientConnection(int fd);
    ~ClientConnection();

    int getFd() const;
    std::string &getBuffer();  // Ritorna il buffer (modificabile)
    
    // Restituisce true se il buffer contiene almeno una richiesta completa
    bool hasCompleteRequest() const;
    
    // Rimuove dal buffer i byte già processati (es. una richiesta completata)
    void removeProcessedRequest(size_t processedLength);

private:
    int fd;
    std::string buffer;
};

#endif
