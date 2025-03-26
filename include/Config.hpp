#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include "Server.hpp"

//queste due strutture non sono più utilizzate
// Config per una "location" (blocco simile; -> e.s. location in NGINX)
struct LocationConfig {
    std::string path;                      // Es. "/" o "/api"
    std::string root;                      // Cartella da cui servire i file
    std::vector<std::string> index;        // File di default (es. index.html)
    bool autoindex;                        // Abilitazione della directory listing
    std::vector<std::string> methods;      // Metodi HTTP accettati (GET, POST, DELETE, ...)
    std::string cgi_extension;             // Es. ".php" se previsto il CGI
};

// Configurazione di un server
struct ServerConfig {
    int listen;                            // Porta su cui ascoltare (es. 8080)
    std::string host;                      // Host/IP (es. "127.0.0.1")
    std::string server_name;               // Nome del server (server virtuali)
    std::string error_page;                // Percorso pagina di errore di default
    size_t client_max_body_size;           // Limite per il corpo delle richieste
    std::vector<LocationConfig> locations; // Blocco di location
};

// Configurazione globale: può essere composta da più server (server virtuali)
struct Config {
    std::vector<Server> *servers;
};
//ok
#endif
