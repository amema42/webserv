#include "Tokenizer.hpp"
#include <cctype>
#include <sstream>

Tokenizer::Tokenizer() {}
Tokenizer::~Tokenizer() {}

bool Tokenizer::isSymbol(char c) const {
    return (c == '{' || c == '}' || c == ';');
}

bool Tokenizer::isWhitespace(char c) const {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

std::vector<Token> Tokenizer::tokenize(const std::string &content) {
    std::vector<Token> tokens;
    size_t i = 0;
    while (i < content.size()) {
        if (isWhitespace(content[i])) {
            i++;
            continue;
        }
        // Se è un simbolo
        if (isSymbol(content[i])) {
            Token token;
            token.type = TOKEN_SYMBOL;
            token.value = content[i];
            tokens.push_back(token);
            i++;
            continue;
        }
        // Se è una parola o un numero (token alfanumerico)
        size_t start = i;
        while (i < content.size() && !isWhitespace(content[i]) && !isSymbol(content[i])) {
            i++;
        }
        std::string word = content.substr(start, i - start);
        Token token;
        // Possiamo migliorare il riconoscimento: se tutti i caratteri sono numerici, possiamo definirlo come TOKEN_NUMBER.
        bool isNum = true;
        for (size_t j = 0; j < word.size(); j++) {
            if (!std::isdigit(word[j])) {
                isNum = false;
                break;
            }
        }
        token.type = (isNum ? TOKEN_NUMBER : TOKEN_WORD);
        token.value = word;
        tokens.push_back(token);
    }
    return tokens;
}
