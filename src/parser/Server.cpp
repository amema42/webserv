#include "../../include/webServ.hpp"
#include <algorithm>
#include <iostream>

/*
dentro ogni server ho caricato un vettore di numeri.
contiene i numeri delle macro che si possono ripetere una volta sola dentro ogni server,
ogni voltache viene inserito un field viene "depennato", cosi se è presente due volte lo 
stesso campo ed è gia stato depennato quel numero il parser andrà in errore.
*/
Server::Server()
{
    std::cout << "Server default constructor" << std::endl;
    numbers.push_back(LISTEN_ARG);
    numbers.push_back(SERVER_NAME_ARG);
    numbers.push_back(ROOT_ARG);
    numbers.push_back(INDEX_ARG);
    numbers.push_back(CLIENT_MAX_BODY_SIZE);
}

Server::~Server()
{
    std::cout << "Server destructor" << std::endl;
    return;
}

Server::Server(const Server &inst) : type(inst.type), numbers(inst.numbers), listen(inst.listen), server_name(inst.server_name), root(inst.root), error_page(inst.error_page), client_max_body_size(inst.client_max_body_size), index(inst.index), location(inst.location)
{
    std::cout << "Server copy constructor" << std::endl;
    return;
}

Server &Server::operator=(const Server &inst)
{
    if (this != &inst) {
        type = inst.type;
        numbers = inst.numbers;
        listen = inst.listen;
        server_name = inst.server_name;
        root = inst.root;
        error_page = inst.error_page;
        client_max_body_size = inst.client_max_body_size;
        index = inst.index;
        location = inst.location;
    }
    return *this;
}


/*
controlla se il Field è gia stato messo in questo server, se nonn c'è lo depenna, se gia c'era, ritorna false
*/
bool Server::setNumberToZero(int number) 
{
	if (number != 21 && number != 31 && number != 41 && number != 69 && number != 71)
		return true;
    std::vector<int>::iterator it = std::find(numbers.begin(), numbers.end(), number);
    if (it != numbers.end()) {
        *it = 0;
        return true;
    }
    return false;
}

// Implementazione dei setter e getter per i vettori

void Server::addToServerName(const std::string& value) {
    server_name.push_back(value);
}

std::vector<std::string> Server::getServerName() const {
    return server_name;
}

size_t Server::getServerNameSize() const {
    return server_name.size();
}

void Server::addToRoot(const std::string& value) {
    root.push_back(value);
}

std::vector<std::string> Server::getRoot() const {
    return root;
}

size_t Server::getRootSize() const {
    return root.size();
}


void Server::addToIndex(const std::string& value) {
    index.push_back(value);
}

std::vector<std::string> Server::getIndex() const {
    return index;
}

size_t Server::getIndexSize() const {
    return index.size();
}

// Implementazione dei setter e getter per il vettore location
void Server::addLocation(const Location& loc) {
    location.push_back(loc);
}

std::vector<Location>& Server::getLocation() {
    return location;
}

const std::vector<Location>& Server::getLocation() const {
    return location;
}