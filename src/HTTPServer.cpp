#include "HTTPServer.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

#include <sstream>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <poll.h>

// #including libraries /headers for socket programming
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

HTTPServer::HTTPServer(const Config &config) : _config(config) {
    // Init degli socket in base alla configurazione
    initSockets();
}

HTTPServer::~HTTPServer() {
    // Close * i socket di ascolto
    for (size_t i = 0; i < _listenSockets.size(); ++i) {
        close(_listenSockets[i]);
    }
}

void HTTPServer::initSockets() {
    // Per ogni **ServerConfig**, crea un socket e configuralo in modalità "non bloccante".
    // Se nel file di configurazione hai più server, vanno gestite eventuali duplicazioni di host:port (like subject specifies: il primo per **host:port** è quello di default).
    for (size_t i = 0; i < _config.servers.size(); ++i) {
        const ServerConfig &server = _config.servers[i];

        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
            throw std::runtime_error("Errore nella creazione del socket.");

        // Configura il socket in modalità non bloccante e imposta FD_CLOEXEC
        int flags = fcntl(sockfd, F_GETFL, 0);
        if (flags < 0)
            throw std::runtime_error("Errore in fcntl F_GETFL");
        if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0)
            throw std::runtime_error("Errore in fcntl F_SETFL");

        // Configura l'indirizzo
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(server.listen);
        addr.sin_addr.s_addr = INADDR_ANY;  // Per semplicità possiamo gestire host specifici in base a server.host 
                                            //(if server.host == "a specific host") do something specific {else} if(server.host == "a specific host") do something else
        memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

        // Lega il socket
        if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            close(sockfd);
            throw std::runtime_error("Errore nel binding del socket.");
        }

        // Inizia ad ascoltare
        if (listen(sockfd, 128) < 0) {
            close(sockfd);
            throw std::runtime_error("Errore in listen().");
        }

        _listenSockets.push_back(sockfd);
        std::cout << "Server in ascolto su porta " << server.listen << std::endl;
    }
}

void HTTPServer::handleClientRequest(int client_fd, const std::string &rawRequest) {
    // 1. Creo un "oggetto" per il parsing della richiesta
    HTTPRequest request;
    request.parseRequest(rawRequest);

    // 2. Costruisco una risposta di base
    HTTPResponse response;
    response.setStatus(200, "OK");
    response.setHeader("Content-Type", "text/html");
    
    // response example: pagina HTML statica
    response.body = "<html><body><h1>Ciao, Mondo!</h1></body></html>";
    
    // set the **Content-Length** (C++98 usa std::ostringstream per la conversione)
    std::ostringstream oss;
    oss << response.body.size();
    response.setHeader("Content-Length", oss.str());

    // Ottieni la stringa completa della risposta
    std::string responseStr = response.toString();

    // Sendind the response... (Hopefully:)
    ssize_t sent = write(client_fd, responseStr.c_str(), responseStr.size());
    if (sent < 0) {
        std::cerr << "Errore in write(): " << strerror(errno) << std::endl;
    }

    // After sending the response , closing the connection (***Va gestito keep-alive?***)
    close(client_fd);
}

void HTTPServer::eventLoop() {
    // Qui utilizzare poll() (or equal func) per gestire le I/O su:
    // - Socket di ascolto*
    // - Connessioni attive*
    // 1. Costruire una struttura (ad esempio std::vector<pollfd>) per gestire eventi di lettura e scrittura.

    // Ad es.:

    std::vector<pollfd> pollfds;
    // Aggiungi i socket di ascolto a pollfds
    for (size_t i = 0; i < _listenSockets.size(); ++i) {
        pollfd pfd;
        pfd.fd = _listenSockets[i];
        pfd.events = POLLIN;
        pfd.revents = 0;
        pollfds.push_back(pfd);
    }
    
    std::cout << "Inizio del loop degli eventi..." << std::endl;

    while (true)
    {
        int ret = poll(&pollfds[0], pollfds.size(), -1);
        if (ret < 0)
        {
            std::cerr << "Errore in poll(): " << strerror(errno) << std::endl;
            continue;
        }
        // Gestire gli eventi: se uno dei socket di ascolto è pronto, accetta nuove connessioni,
        // altrimenti gestire le connessioni esistenti.

        // Itera sui pollfd per verificare quali sono pronti
        for (size_t i = 0; i < pollfds.size(); ++i)
        {
            if (pollfds[i].revents & POLLIN)
            {
                // Se il **socket è di ascolto**: **Accetta nuove connessioni**
                if (std::find(_listenSockets.begin(), _listenSockets.end(), pollfds[i].fd) != _listenSockets.end())
                {
                    // Accettiamo la connessione
                    sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_fd = accept(pollfds[i].fd, (struct sockaddr*)&client_addr, &client_len);
                    if (client_fd < 0)
                    {
                        std::cerr << "Errore in accept(): " << strerror(errno) << std::endl;
                        continue;
                    }

                    // Imposta il socket del client in modalità non bloccante
                    int flags = fcntl(client_fd, F_GETFL, 0);
                    if (flags >= 0)
                    {
                        fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
                    }
                    
                    std::cout << "Nuova connessione accettata: fd " << client_fd << std::endl;

                    // Aggiungi il client alla lista dei pollfd per monitorarlo
                    struct pollfd client_pfd;
                    client_pfd.fd = client_fd;
                    client_pfd.events = POLLIN;  // Monitoriamo per lettura (richieste HTTP)
                    client_pfd.revents = 0;
                    pollfds.push_back(client_pfd);
                }
                else
                {
                    // Qui gestiresti la comunicazione con un client già connesso
                    // Ad esempio, leggere la richiesta HTTP
                    char buffer[1024];
                    ssize_t n = read(pollfds[i].fd, buffer, sizeof(buffer) - 1);
                    if (n <= 0)
                    {
                        if (n < 0)
                            std::cerr << "Errore in read(): " << strerror(errno) << std::endl;
                        else
                            std::cout << "Chiusura della connessione: fd " << pollfds[i].fd << std::endl;
                        close(pollfds[i].fd);
                        // Rimuovi il pollfd dalla lista
                        pollfds.erase(pollfds.begin() + i);
                        --i;
                    }
                    else
                    {
                        buffer[n] = '\0';
                        std::string rawRequest(buffer);
                        std::cout << "Ricevuto dal fd " << pollfds[i].fd << ": " << rawRequest << std::endl; /* Buffer -> rawRequest*/
                        
                        // chiamo handleClientRequest per processare la richiesta + inviare la risposta
                        handleClientRequest(pollfds[i].fd, rawRequest);
                    
                        // Rimuoviamo il client dalla lista; handleClientRequest ha chiuso il socket
                        pollfds.erase(pollfds.begin() + i);
                        --i;

                    }
                }
            }
        }
    }
}

void HTTPServer::run() {
    // Start loop degli eventi
    eventLoop();
}

// **La gestione del loop degli eventi, da subject:**
// Usare solamente poll() per gestire le I/O su socket. (gestire in modo non bloccante LETTURA e SCRITTURA)
// & Gestire eventuali errori senza terminare il server