#include "webServ.hpp"


int main(int ac, char **av){
	std::string confPath = "../config/webserv.conf";
	if (ac > 1)
		confPath = av[1];
	else if(ac > 2){
		std::cout << "add just the config path please"<< std::endl;
		return -1;
	}
	std::vector<Server> servers;
	//parsa il config file
	if(!ParseFileLineByLine(confPath, servers));
		return -1;

	//chiusura del tutto
	return 0;
}
