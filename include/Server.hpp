#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include "webServ.hpp"

class Location;

class Server
{
    private:
        std::string type;
        std::vector<int> numbers;

    public:
        std::vector<int> listen;
        std::vector<std::string> server_name;
        std::vector<std::string> root;
        std::map<std::string, std::string> error_page;
        std::vector<size_t> client_max_body_size;
        std::vector<std::string> index;
        std::vector<Location> location;

        ~Server(); //def dest
        Server(); //def cost
        Server(const Server &inst); //copy const
        Server &operator=(const Server &inst); // = op
        bool setNumberToZero(int number);

        // Setter e getter per i vettor
        void addToServerName(const std::string& value);
        std::vector<std::string> getServerName() const;
        size_t getServerNameSize() const;

        void addToRoot(const std::string& value);
        std::vector<std::string> getRoot() const;
        size_t getRootSize() const;

        void addToErrorPage(const std::vector<std::string>& value);
        std::vector<std::vector<std::string> > getErrorPage() const;
        size_t getErrorPageSize() const;


        void addToIndex(const std::string& value);
        std::vector<std::string> getIndex() const;
        size_t getIndexSize() const;

        // Setter e getter per il vettore location
        void addLocation(const Location& loc);
        std::vector<Location>& getLocation();
        const std::vector<Location>& getLocation() const;
};

#endif