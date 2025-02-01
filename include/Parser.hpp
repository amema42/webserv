#ifndef PARSER_HPP
#define PARSER_HPP

#include "Config.hpp"
#include "Tokenizer.hpp"
#include <vector>
#include <string>

class Parser {
public:
    Parser();
    ~Parser();

    // Funzione principale per parsare il contenuto del file di configurazione
    // Ritorna una struttura Config che contiene la configurazione completa
    Config parse(const std::string &configContent);

private:
    // Funzioni helper per il parsing
    ServerConfig parseServer(const std::vector<Token> &tokens, size_t &pos);
    LocationConfig parseLocation(const std::vector<Token> &tokens, size_t &pos);
    void expectToken(const std::vector<Token> &tokens, size_t &pos, const std::string &expected);

    // Funzione helper per gestire direttive (es. "listen", "host", ecc.)
    void parseDirective(ServerConfig &server, const std::vector<Token> &tokens, size_t &pos);
    void parseDirective(LocationConfig &location, const std::vector<Token> &tokens, size_t &pos);
};

#endif
