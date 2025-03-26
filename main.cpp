#include "webServ.hpp"

int main(int ac, char **av){
	std::string confPath = "./config/webserv.conf";
	if (ac > 1)
		confPath = av[1];
	else if(ac > 2){
		std::cout << "add just the config path please"<< std::endl;
		return -1;
	}
	std::vector<Server> servers;
	//parsa il config file
	if(!ParseFileLineByLine(confPath, servers))
		return -1;
	Config config;
	config.servers = &servers;
	//creare una funzione di stampa
	for (size_t i = 0; i < config.servers->size(); i++) {
		std::cout << "Server " << i+1 << ":\n";
		std::cout << "  |-Listen: " << mcamilli((*config.servers)[i].listen) << "\n";
		std::cout << "  |-Server Name: " << mcamilli((*config.servers)[i].server_name) << "\n";
		std::cout << "  |-Client Max Body Size: " << mcamilli((*config.servers)[i].client_max_body_size) << "\n";
		std::cout << "  |-Server root: " << mcamilli((*config.servers)[i].root) << std::endl;
		std::cout << "  |-Server index: " << mcamilli((*config.servers)[i].index) << std::endl;
		std::cout << "  |-Numero di location: "<< (*config.servers)[i].location.size() << std::endl;
		for (size_t j = 0; j < (*config.servers)[i].location.size(); j++){
			std::cout << "\tlocation n:  "<< j << std::endl;
			std::cout << "\t  |-path:  " << mcamilli((*config.servers)[i].location[j].path) << std::endl;
			std::cout << "\t  |-root:  " << mcamilli((*config.servers)[i].location[j].l_root) << std::endl;
			std::cout << "\t  |-l_index " << mcamilli((*config.servers)[i].location[j].l_index) << std::endl;
		}
	}
	try{
		HTTPServer webServ(config);
		webServ.run();
	} catch (std::exception &e) {
		std::cerr << "Errore: " << e.what() << std::endl;
		return 1;
	}

	//chiusura del tutto
	return 0;
}
