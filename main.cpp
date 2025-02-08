#include <vector>
#include <iostream>
#include "webserv.hpp" // Assicurati di avere un file di intestazione per la classe Server

int main() 
{
    std::vector<Server> servers;
    std::string path;
	
    // Chiama una funzione per aggiungere server al vettore
    ParseFileLineByLine(path, servers);


    return 0;
}