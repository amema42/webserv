#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "webServ.hpp"

class Location
{
    private:
        
    public:
		bool autoindex_flag;
		std::string autoindex;
        std::vector<int> numbers;
		std::vector<std::string> path;
        std::vector<std::string> l_root;
        std::vector<std::string> l_index;
        std::vector<std::string> l_methods;
        std::vector<std::string> l_upload_store;
		std::vector<size_t> l_client_max_body_size;
		std::vector<std::string> redirect_page;
        

        ~Location(); //def dest
        Location(); //def cost
        Location(const Location &inst); //copy const
        Location &operator=(const Location &inst); // = op
        bool setNumberToZero(int number);

        void addToLMethods(const std::string& value);
};

#endif