#include "Parser.hpp"
#include <cstdlib>
#include <stdexcept>
#include <iostream>

Parser::Parser() {}
Parser::~Parser() {}

Config Parser::parse(const std::string &configContent) {
    Config config;
    // Otteniamo la lista dei token
    Tokenizer tokenizer;
    std::vector<Token> tokens = tokenizer.tokenize(configContent);
    size_t pos = 0;
    while (pos < tokens.size()) {
        // Ci aspettiamo la direttiva "server"
        if (tokens[pos].value == "server") {
            ServerConfig server = parseServer(tokens, pos);
            config.servers.push_back(server);
        } else {
            // Se troviamo qualcosa che non è "server", può essere un commento o un errore
            pos++;
        }
    }
    return config;
}

void Parser::expectToken(const std::vector<Token> &tokens, size_t &pos, const std::string &expected) {
    if (pos >= tokens.size() || tokens[pos].value != expected) {
        throw std::runtime_error("Errore di parsing: ci si aspettava '" + expected + "' ma trovato '" +
                                 (pos < tokens.size() ? tokens[pos].value : "EOF") + "'");
    }
    pos++;
}

ServerConfig Parser::parseServer(const std::vector<Token> &tokens, size_t &pos) {
    ServerConfig server;
    // Ci aspettiamo "server"
    if (tokens[pos].value != "server") {
        throw std::runtime_error("Errore: direttiva 'server' attesa.");
    }
    pos++;
    // Aspettiamo '{'
    expectToken(tokens, pos, "{");

    // Leggiamo direttive fino a che non troviamo '}'
    while (pos < tokens.size() && tokens[pos].value != "}") {
        if (tokens[pos].value == "listen") {
            pos++;
            server.listen = std::atoi(tokens[pos].value.c_str());
            pos++;
            expectToken(tokens, pos, ";");
        } else if (tokens[pos].value == "host") {
            pos++;
            server.host = tokens[pos].value;
            pos++;
            expectToken(tokens, pos, ";");
        } else if (tokens[pos].value == "server_name") {
            pos++;
            server.server_name = tokens[pos].value;
            pos++;
            expectToken(tokens, pos, ";");
        } else if (tokens[pos].value == "error_page") {
            pos++;
            server.error_page = tokens[pos].value;
            pos++;
            expectToken(tokens, pos, ";");
        } else if (tokens[pos].value == "client_max_body_size") {
            pos++;
            server.client_max_body_size = static_cast<size_t>(std::atoi(tokens[pos].value.c_str()));
            pos++;
            expectToken(tokens, pos, ";");
        } else if (tokens[pos].value == "location") {
            // Gestiamo il blocco location
            pos++;
            // Il token successivo è il path della location
            LocationConfig location;
            location.path = tokens[pos].value;
            pos++;
            // Aspettiamo '{'
            expectToken(tokens, pos, "{");
            location = parseLocation(tokens, pos);
            server.locations.push_back(location);
        } else {
            // Se troviamo una direttiva sconosciuta, la saltiamo (oppure gestisci l'errore)
            std::cerr << "Direttiva sconosciuta nel blocco server: " << tokens[pos].value << std::endl;
            pos++;
        }
    }
    expectToken(tokens, pos, "}"); // Fine del blocco server
    return server;
}

LocationConfig Parser::parseLocation(const std::vector<Token> &tokens, size_t &pos) {
    LocationConfig location;
    // Inizializziamo alcuni valori di default
    location.autoindex = false;
    while (pos < tokens.size() && tokens[pos].value != "}") {
        if (tokens[pos].value == "root") {
            pos++;
            location.root = tokens[pos].value;
            pos++;
            expectToken(tokens, pos, ";");
        } else if (tokens[pos].value == "index") {
            pos++;
            // Potrebbe esserci più di un file index
            while (tokens[pos].value != ";") {
                location.index.push_back(tokens[pos].value);
                pos++;
            }
            expectToken(tokens, pos, ";");
        } else if (tokens[pos].value == "autoindex") {
            pos++;
            location.autoindex = (tokens[pos].value == "on");
            pos++;
            expectToken(tokens, pos, ";");
        } else if (tokens[pos].value == "methods") {
            pos++;
            // Leggiamo tutti i metodi fino al ';'
            while (tokens[pos].value != ";") {
                location.methods.push_back(tokens[pos].value);
                pos++;
            }
            expectToken(tokens, pos, ";");
        } else if (tokens[pos].value == "cgi") {
            pos++;
            location.cgi_extension = tokens[pos].value;
            pos++;
            expectToken(tokens, pos, ";");
        } else {
            std::cerr << "Direttiva sconosciuta nel blocco location: " << tokens[pos].value << std::endl;
            pos++;
        }
    }
    expectToken(tokens, pos, "}");
    return location;
}
