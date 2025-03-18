#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include "webServ.hpp"
#include "Config.hpp"
#include "ClientConnection.hpp"
#include <vector>


class HTTPServer {
public:
    HTTPServer(const Config &config);
    ~HTTPServer();

    // Avvia il server: (1) inizializza i "socket", (2) "il loop degli eventi", etc.
    void run();

private:
    Config _config;
    // ToDo: costruire qui una struttura per gestire (ogni) socket di ascolto
    std::vector<int> _listenSockets;

    std::vector<ClientConnection*> _clientConnections; //container per le connessioni client attive

    // helper functions
    void initSockets();
    void eventLoop();

    // Metodo per gestire "una" richiesta del client -> Riceve *fd* del client e la richiesta raw/grezza (rawRequest)
    //void handleClientRequest(int client_fd, const std::string &rawRequest); -> modifico la funzione in modo da ricevere un puntatore a ClientConnection invece del solo fd.
    // logica rimane simile, ma va verificata la presenza dell'header Connection: keep-alive.
    void handleClientRequest(ClientConnection *clientConn, const std::string &rawRequest);

};

#endif
