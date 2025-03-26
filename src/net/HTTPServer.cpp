#include "webServ.hpp"
#include "HTTPServer.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "ClientConnection.hpp"


#include <sstream>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <poll.h>
#include <cerrno>
#include <algorithm>


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
    for (size_t i = 0; i < (*_config.servers).size(); ++i) {
        const Server &server = (*_config.servers)[i];

        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
            throw std::runtime_error("Error in socket creation.");

        // Configura il socket in modalità non bloccante e imposta FD_CLOEXEC
        int flags = fcntl(sockfd, F_GETFL, 0);
        if (flags < 0)
            throw std::runtime_error("Errore in fcntl F_GETFL");
        if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0)
            throw std::runtime_error("Errore in fcntl F_SETFL");

        // Configura l'indirizzo
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(server.listen[0]);
        addr.sin_addr.s_addr = INADDR_ANY;  // Per semplicità possiamo gestire host specifici in base a server.host 
                                            //(if server.host == "a specific host") do something specific {else} if(server.host == "a specific host") do something else
        memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

        // Lega il socket
        if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            close(sockfd);
            throw std::runtime_error("Error binding socket.");
        }

        // Inizia ad ascoltare
        if (listen(sockfd, 128) < 0) {
            close(sockfd);
            throw std::runtime_error("Error in listen().");
        }

        _listenSockets.push_back(sockfd);
        std::cout << "Server are listening on port " << server.listen[0] << std::endl;
    }
}

void HTTPServer::handleGetRequest(const HTTPRequest& request, HTTPResponse& response) {
    std::stringstream filepath;
    getServerByHost(request, _config);
    if(request.uri.size() == 1)
        filepath << (*_config.servers)[0].root[0] << request.uri  << (*_config.servers)[0].index[0];
    else
        filepath << (*_config.servers)[0].root[0] << request.uri << "/" << (*_config.servers)[0].index[0];
    std::cout << "\t\tla path per il file da trovare" << filepath.str() << std::endl;
    try {
        // std::string contentType = getMimeType(filePath); // Implementa mappa MIME
        std::string content = readFile(filepath.str());
        response.setStatus(200, "OK");
        response.body = content;
    }
    catch (std::exception& e){
        std::cout << e.what() << std::endl;
        response.setStatus(404, "Not Found");
        return;
    }
    return;
    
}

void HTTPServer::handleClientRequest(ClientConnection *clientConn, const std::string &rawRequest) {
    // 1. Creo un "oggetto" per il parsing della richiesta ed uno per la risposta!!!
    HTTPRequest request;
    HTTPResponse response;
    
    request.parseRequest(rawRequest);
    // 2. Costruiamo la rispostaaaa
    if(request.method != "GET" && request.method != "POST" && request.method != "DELETE")
        response.setStatus(400, "bad request");
    else
        response.setStatus(200, "OK");
    
    //gestione dei varii casi
    if (request.uri.find("cgi-bin") != std::string::npos){
        std::cout << "handle cgi request" << std::endl;
        CGIHandler cgi(request.uri);
        try{
            response.body = cgi.executeScript(request.method, request.body);
            std::cout << response.body << std::endl;
        }
        catch (std::exception& e){
            std::cout << e.what() <<std::endl;
        }
    }
    
    else if (request.method == "GET"){
        std::cout << "handle get request" << std::endl;
        handleGetRequest(request, response);
    }
    else if (request.method == "POST"){
        std::cout<< "handle POST request" << std::endl;
        //handlepost();
        response.body = "<html><body><h1>Ciao, Mondo dal post!</h1></body></html>";
    }
    else {
        std::cout << "handle DELETE request" << std::endl;
        response.body = "<html><body><h1>Ciao, Mondo!</h1></body></html>";
    }
    response.setHeader("Content-Type", "text/html");
    // response example: pagina HTML statica
    // bisogna fornire la pagina richiesta 
    
    // set the **Content-Length** (C++98 usa std::ostringstream per la conversione)
    std::ostringstream oss;
    oss << response.body.size();
    response.setHeader("Content-Length", oss.str());

    // Ottieni la stringa completa della risposta
    std::string responseStr = response.toString();

    // Sendind the response... (Hopefully:)
    ssize_t sent = write(clientConn->getFd(), responseStr.c_str(), responseStr.size());
    if (sent < 0) {
        std::cerr << "Errore in write(): " << strerror(errno) << std::endl;
    }

    // After sending the response , closing the connection (***Va gestito keep-alive?***)
    //close(client_fd);
    /* bool keepAlive = false; r.114: part of: keep-alive connection implem. */ 
    /* Qui viene verificato se IL CLIENT CHIEDE KEEP-ALIVEW
    if (richiesta contiene "Connection: keep-alive" negli header) :NON chiudiamo la connessione
    else :chiudiamo la connessione
    */
    bool keepAlive = false; 
    std::map<std::string, std::string>::iterator it = request.headers.find("Connection");
    if (it != request.headers.end()) {
        std::string conn = it->second;
        // Convert w. tolower to avoid case sensitivity 
        for (size_t i = 0; i < conn.size(); ++i) {
            conn[i] = tolower(conn[i]);
        }
        if (conn == "keep-alive")
            keepAlive = true;
    }

    if (!keepAlive) {
        // Chiudiamo la connessione se non è keep-alive && in caso di keep-alive dopo aver processato la richiesta, va rimossa dal buffer SOLO la parte processata.
        close(clientConn->getFd());
    }

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
    
    std::cout << "start event loop..." << std::endl;

    while (true)
    {
        int ret = poll(&pollfds[0], pollfds.size(), -1);
        if (ret < 0)
        {
            std::cerr << "Error in poll(): " << strerror(errno) << std::endl;
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
                        std::cerr << "Error in accept(): " << strerror(errno) << std::endl;
                        continue;
                    }

                    // Imposta il socket del client in modalità non bloccante
                    int flags = fcntl(client_fd, F_GETFL, 0);
                    if (flags >= 0)
                    {
                        fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
                    }
                    
                    std::cout << "new connection acceted: fd " << client_fd << std::endl;

                    ClientConnection *newConn = new ClientConnection(client_fd); // Creo un nuovo oggetto ClientConnection per il "nuovo" fd
                    _clientConnections.push_back(newConn);

                    // Aggiungi il client alla lista dei pollfd per monitorarlo
                    pollfd client_pfd; /*old ver: struct pollfd client_pfd;*/
                    client_pfd.fd = client_fd;
                    client_pfd.events = POLLIN;  // Monitoriamo per lettura (richieste HTTP)
                    client_pfd.revents = 0;
                    pollfds.push_back(client_pfd);
                }
                else
                {
                    // Qui gestiresti la comunicazione con un client già connesso
                    // Ad esempio, leggere la richiesta HTTP
                    
                    ClientConnection *conn = NULL; // Il fd appartiene a un client già connesso -> Troviamo l'oggetto ClientConnection corrispondente
                    for (size_t j = 0; j < _clientConnections.size(); ++j) {
                        if (_clientConnections[j]->getFd() == pollfds[i].fd) {
                            conn = _clientConnections[j];
                            break;
                        }
                    }
                    if (!conn) continue;

                    char buffer[1024];
                    ssize_t n = read(pollfds[i].fd, buffer, sizeof(buffer) - 1);
                    if (n <= 0)
                    {
                        if (n < 0)
                            std::cerr << "Error in read(): " << strerror(errno) << std::endl;
                        else
                            std::cout << "closing connection: fd " << pollfds[i].fd << std::endl;
                        close(pollfds[i].fd);
                        // Rimuovi il pollfd dalla lista + e l'oggetto ClientConnection
                        pollfds.erase(pollfds.begin() + i);
                        _clientConnections.erase(
                            std::remove(_clientConnections.begin(), _clientConnections.end(), conn),
                            _clientConnections.end());
                        delete conn;
                        --i;
                    }
                    else
                    {
                        buffer[n] = '\0';
                        // Add i dati letti al buffer persistente della connection
                        conn->getBuffer().append(buffer);
                        std::cout << "Buffer for fd " << conn->getFd() << ": " << conn->getBuffer() << std::endl;
                        if (conn->hasCompleteRequest()) {
                            // Qui, per semplicità, consideriamo l'intero buffer come una richiesta
                            std::string rawRequest = conn->getBuffer();
                            std::cout << "complete request from fd " << conn->getFd() << ": " << rawRequest << std::endl;
                        
                        handleClientRequest(conn, rawRequest); // Processa la richiesta
                        /*Se il client non usa keep-alive, handleClientRequest ha chiuso il socket.
                        Se invece usa keep-alive, dovrai rimuovere solo la parte processata.
                        Qui, per semplicità, chiudiamo la connessione dopo aver inviato la risposta:
                        In una versione avanzata, dovresti analizzare la richiesta e mantenere il buffer.*/

                        // Rimuoviamo il ClientConnection e il relativo pollfd.
                        for (size_t k = 0; k < pollfds.size(); ++k) 
                        {
                            if (pollfds[k].fd == conn->getFd()) 
                            {
                                pollfds.erase(pollfds.begin() + k);
                                break;
                            }
                        }
                        _clientConnections.erase(
                            std::remove(_clientConnections.begin(), _clientConnections.end(), conn),
                            _clientConnections.end());
                        delete conn;
                            //l'indice per la rimozione
                        --i;

                    }
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