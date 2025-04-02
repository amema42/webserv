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

Server::Server(const Server &inst) : type(inst.type), numbers(inst.numbers), listen(inst.listen), server_name(inst.server_name), root(inst.root), error_page(inst.error_page), client_max_body_size(inst.client_max_body_size), index(inst.index), location(inst.location),  l_cgi_path(inst.l_cgi_path), l_cgi_extension(inst.l_cgi_extension)
{
    std::cout << "Server copy constructor" << std::endl;
    return;
}

Server &Server::operator=(const Server &inst)
{
    if (this != &inst) 
	{
        type = inst.type;
        numbers = inst.numbers;
        listen = inst.listen;
        server_name = inst.server_name;
        root = inst.root;
        error_page = inst.error_page;
        client_max_body_size = inst.client_max_body_size;
        index = inst.index;
        location = inst.location;
		l_cgi_extension = inst.l_cgi_extension;
		l_cgi_path = inst.l_cgi_path;
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

std::string Server::getErrorPage(std::string value)
{

    std::map<std::string, std::string>::iterator it =  error_page.find(value);
    if(it != error_page.end())
        return (it->second);
    throw std::runtime_error("no errr page for code: " + value);
}