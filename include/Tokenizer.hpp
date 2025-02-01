#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <string>
#include <vector>

enum TokenType {
    TOKEN_WORD,
    TOKEN_NUMBER,
    TOKEN_SYMBOL // esempio: '{', '}', ';' & more
};

struct Token {
    TokenType type;
    std::string value;
};

class Tokenizer {
public:
    Tokenizer();
    ~Tokenizer();

    // Legge il file di configurazione e restituisce la lista di token
    std::vector<Token> tokenize(const std::string &content);

private:
    // Funzioni helper per gestire la tokenizzazione
    bool isSymbol(char c) const;
    bool isWhitespace(char c) const;
};

#endif
