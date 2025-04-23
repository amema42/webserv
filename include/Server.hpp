#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include "webServ.hpp"

class Location;

class Server
{
    private:
        std::vector<int> numbers;

    public:
        std::vector<int> listen;
        std::vector<std::string> server_name;
        std::vector<std::string> root;
        std::map<std::string, std::string> error_page;
        std::vector<size_t> client_max_body_size;
        std::vector<std::string> index;
        std::vector<Location> location;
		std::vector<std::string> l_cgi_path;
        std::vector<std::string> l_cgi_extension;

        ~Server(); //def dest
        Server(); //def cost
        Server(const Server &inst); //copy const
        Server &operator=(const Server &inst); // = op
        bool setNumberToZero(int number);

        // Setter e getter per i vettor
    	std::string getErrorPage(std::string value);
};

#endif