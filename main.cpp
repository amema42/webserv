#include "webServ.hpp"
volatile sig_atomic_t webserv_runo = 1;

bool ends_with_conf(const std::string& str)
{
	std::cout << str.substr(str.length() - 5, str.length()) <<std::endl;
	if (str.substr(str.length() - 5, str.length()) != ".conf")
		return false;
	else
		return true;
}

std::string config_example()
{
	std::string config_str =
"#hi this is our webserver, this is an example of valid config:\n"
"# try the command './webserv > valid.conf' and then './webserv valid.conf'\n"
"server {\n"
"    listen 8080;\n"
"    server_name mysite.com;\n"
"    root www;\n"
"    index index.html;\n"
"    cgi_path www/cgi-bin;\n"
"    cgi_extension .py;\n"
"    client_max_body_size 5M;\n"
"    error_page 405 www/error/405.html;\n"
"    error_page 404 www/error/404.html;\n"
"    location /get/ {\n"
"        return 301 http://youtube.com/shorts/KUM2p2Weicg;\n"
"        index index2.html;\n"
"        autoindex off;\n"
"    }\n"
"    location /post {\n"
"        index index.html;\n"
"    }\n"
"    location /uploads/ {\n"
"        index index.html;\n"
"        methods GET POST DELETE;\n"
"    }\n"
"    location /about {\n"
"        index index.html;\n"
"    }\n"
"}\n"
"\n"
"server {\n"
"    listen 8081;\n"
"    server_name mysite2.com;\n"
"    root www;\n"
"    index index1.html;\n"
"    client_max_body_size 5M;\n"
"\n"
"    location /get {\n"
"        index index.html;\n"
"    }\n"
"}\n";

	return(config_str);
}

int main(int ac, char **av){
	std::string confPath = "./config/webserv.conf";
	if (ac < 2) 
	{
		std::cout <<  config_example() << std::endl;
		return -1;
	} 
	else if (ac == 2)
	{
		confPath = av[1];
	}
	if (!ends_with_conf(confPath))
	{
		std::cout << "file must be .conf" << std::endl;
		return -1;
	}
	std::vector<Server> servers;
	//config file pars
	if(!ParseFileLineByLine(confPath, servers))
		return -1;
	Config config;
	config.servers = &servers;
	//creare funzione di stampa;
	//std::cout << servers[0].l_cgi_extension.back() << "\n";
	for (size_t i = 0; i < config.servers->size(); i++) {
		std::cout << "Server " << i+1 << ":\n";
		std::cout << "  |-Listen: " << mcamilli((*config.servers)[i].listen) << "\n";
		std::cout << "  |-Server Name: " << mcamilli((*config.servers)[i].server_name) << "\n";
		std::cout << "  |-Client Max Body Size: " << mcamilli((*config.servers)[i].client_max_body_size) << "\n";
		std::cout << "  |-Server root: " << mcamilli((*config.servers)[i].root) << std::endl;
		std::cout << "  |-Server index: " << mcamilli((*config.servers)[i].index) << std::endl;
		std::cout << "  |-Server cgi_path: " << mcamilli((*config.servers)[i].l_cgi_path) << std::endl;
		std::cout << "  |-Server cgi_ext: " << mcamilli((*config.servers)[i].l_cgi_extension) << std::endl;
		std::cout << "  |-Numero di location: "<< (*config.servers)[i].location.size() << std::endl;
		std::cout << "  |-Server index: " << mcamilli((*config.servers)[i].index) << std::endl;
		std::cout << "  |-Error pages:" << std::endl;
		printMap((*config.servers)[i].error_page, "   |-error: ", "page: ");
		for (size_t j = 0; j < (*config.servers)[i].location.size(); j++){
			std::cout << "\tlocation n:  "<< j << std::endl;
			std::cout << "\t  |-path:  " << mcamilli((*config.servers)[i].location[j].path) << std::endl;
			std::cout << "\t  |-root:  " << mcamilli((*config.servers)[i].location[j].l_root) << std::endl;
			std::cout << "\t  |-l_index " << mcamilli((*config.servers)[i].location[j].l_index) << std::endl;
			std::cout << "\t  |-autoindex " << servers[i].location[j].autoindex << std::endl;
			//printMap((*config.servers)[i].location[j].redirect_page, "   |-redirection code: ", "page: ");
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
