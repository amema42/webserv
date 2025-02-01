#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include "Config.hpp"
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

    // helper functions
    void initSockets();
    void eventLoop();
};

#endif
