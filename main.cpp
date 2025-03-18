#include "webServ.hpp"


void printServerDetails(const std::vector<Server>& servers) {
	for (size_t i = 0; i < servers.size(); ++i) {
		const Server& server = servers[i];
		std::cout << "Server " << i + 1 << ":" << std::endl;

		std::cout << "  Listen:" << std::endl;
		for (const auto& item : server.listen) {
			std::cout << "    " << item << std::endl;
		}

		std::cout << "  Server Name:" << std::endl;
		for (const auto& item : server.server_name) {
			std::cout << "    " << item << std::endl;
		}

		std::cout << "  Root:" << std::endl;
		for (const auto& item : server.root) {
			std::cout << "    " << item << std::endl;
		}

		std::cout << "  Error Page:" << std::endl;
		for (const auto& error : server.error_page) {
			std::cout << "    Error Code: " << error.first << " -> " << error.second << std::endl;
		}

		std::cout << "  Client Max Body Size:" << std::endl;
		for (const auto& item : server.client_max_body_size) {
			std::cout << "    " << item << std::endl;
		}

		std::cout << "  Index:" << std::endl;
		for (const auto& item : server.index) {
			std::cout << "    " << item << std::endl;
		}

		std::cout << "  Locations:" << std::endl;
		for (size_t j = 0; j < server.location.size(); ++j) {
			const Location& location = server.location[j];
			std::cout << "    Location " << j + 1 << ":" << std::endl;

			std::cout << "      Path:" << std::endl;
			for (const auto& item : location.path) {
				std::cout << "        " << item << std::endl;
			}

			std::cout << "      Root:" << std::endl;
			for (const auto& item : location.l_root) {
				std::cout << "        " << item << std::endl;
			}

			std::cout << "      Index:" << std::endl;
			for (const auto& item : location.l_index) {
				std::cout << "        " << item << std::endl;
			}

			std::cout << "      Methods:" << std::endl;
			for (const auto& item : location.l_methods) {
				std::cout << "        " << item << std::endl;
			}

			std::cout << "      CGI Path:" << std::endl;
			for (const auto& item : location.l_cgi_path) {
				std::cout << "        " << item << std::endl;
			}

			std::cout << "      CGI Extension:" << std::endl;
			for (const auto& item : location.l_cgi_extension) {
				std::cout << "        " << item << std::endl;
			}

			std::cout << "      Upload Store:" << std::endl;
			for (const auto& item : location.l_upload_store) {
				std::cout << "        " << item << std::endl;
			}

			std::cout << "      Client Max Body Size:" << std::endl;
			for (const auto& item : location.l_client_max_body_size) {
				std::cout << "        " << item << std::endl;
			}
		}
	}
}

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
	if(!ParseFileLineByLine(confPath, servers))
		return -1;
	Config config;
	config.servers = &servers;
	//creare una funzione di stampa
	for (size_t i = 0; i < config.servers->size(); i++) {
		std::cout << "Server " << i+1 << ":\n";
		std::cout << "  |-Listen: " << (*config.servers)[i].listen[0] << "\n";
		std::cout << "  |-Server Name: " << (*config.servers)[i].server_name[0] << "\n";
		std::cout << "  |-Client Max Body Size: " << (*config.servers)[i].client_max_body_size[0] << "\n";
		std::cout << "  |-Numero di location: "<< (*config.servers)[i].location.size() << std::endl;
		for (size_t j = 0; j < (*config.servers)[i].location.size(); j++){
			std::cout << "\tlocation n:  "<< j << std::endl;
			std::cout << "\t  |-path:  " << (*config.servers)[i].location[j].path[0] << std::endl;
			std::cout << "\t  |-root:  " << (*config.servers)[i].location[j].l_root[0] << std::endl;
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
