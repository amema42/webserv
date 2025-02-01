#include "Parser.hpp"
#include "HTTPServer.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

std::string readFile(const std::string &filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        throw std::runtime_error("Impossibile aprire il file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char **argv) {
    std::string configPath = "config/default.conf"; // percorso di default
    if (argc > 1) {
        configPath = argv[1];
    }

    try {
        std::string configContent = readFile(configPath);
        Parser parser;
        Config config = parser.parse(configContent);
        // Stampa di debug: direttive lette
        std::cout << "Numero di server configurati: " << config.servers.size() << std::endl;
        for (size_t i = 0; i < config.servers.size(); i++) {
            std::cout << "Server " << i+1 << ":\n";
            std::cout << "\tListen: " << config.servers[i].listen << "\n";
            std::cout << "\tHost: " << config.servers[i].host << "\n";
            std::cout << "\tServer Name: " << config.servers[i].server_name << "\n";
            std::cout << "\tError Page: " << config.servers[i].error_page << "\n";
            std::cout << "\tClient Max Body Size: " << config.servers[i].client_max_body_size << "\n";
            std::cout << "\tNumero di location: " << config.servers[i].locations.size() << "\n";
        }
        HTTPServer server(config);
        server.run();

    } catch (std::exception &e) {
        std::cerr << "Errore: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
