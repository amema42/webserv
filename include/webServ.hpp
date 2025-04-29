/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcamilli <mcamilli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 15:06:40 by jacopo            #+#    #+#             */
/*   Updated: 2025/04/29 10:44:42 by mcamilli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#if !defined(WEBSERV_hpp)
#define WEBSERV_hpp


//include all the library here pls 
#include <dirent.h>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <poll.h>
#include <cerrno>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <fstream>
#include <ctime>
#include <string>
#include <vector>
#include <string>
#include <map>

#include "CGIHandler.hpp"
#include "./Server.hpp" 
#include "./Location.hpp" 
#include "ClientConnection.hpp"
#include "./Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "./HTTPServer.hpp"
#include "Tokenizer.hpp"

#define SIZE_MAX 18446744073709551615UL
#define ERROR 0
#define SERVER 1
#define OPEN_S_BRACKET 7
#define IN_SERVER 10
#define LOCATION_PATH 51
#define OPEN_L_BRACKET 57
#define IN_LOCATION 50
#define LISTEN_ARG 21
#define SERVER_NAME_ARG 31 // 1 arg
#define ROOT_ARG 41 // 1 arg
#define INDEX_ARG 69 // quanti ne vuoi
#define ERROR_PAGE_ARG 92 // 2 arg
#define CLIENT_MAX_BODY_SIZE 71 // 1 arg
#define L_ROOT_ARG 5041 //1 arg
#define L_INDEX_ARG 5069 //quanti ne vuoi
#define L_METODS_ARG 5123 //da uno a tre
#define L_CGI_PATH_ARG 5111 //1 arg
#define L_CGI_EXTENSION_ARG 5121 //1 arg
#define L_UPLOAD_STORE_ARG 5131 // 1 arg
#define L_CLIENT_MAX_BODY_SIZE 5071 //1 arg
#define REDIRECTION 5302 //Return 2 arg
#define AUTOINDEX 5001 //1 ARG

bool endsWithSemicolon(const std::string& word);
bool insertArgInField(std::string& Word, int look_for, std::vector<std::string>& args, int n_line);
bool insertInMethods(std::istringstream& iss, std::string& Word, int look_for, Location& location, int n_line);
int ParseFileLineByLine(const std::string& filePath, std::vector<Server>& servers);
std::string readFile(const std::string &filename);

template <typename T>
std::string mcamilli(const std::vector<T>& value){
	std::ostringstream result;
	for (std::size_t i = 0; i < value.size(); i++){
		result << value[i];
	}
	return result.str();
}

Server& getServerByHost(const HTTPRequest& request, const Config & config);
void printMap(std::map<std::string, std::string>& map, std::string first, std::string second);
std::string getHeaderValue(const std::string toFind, const HTTPRequest& request);
std::string CreateFileName(HTTPRequest& request);
Location&  getLocationByName(std::string path, Server& server);

#endif // WEBSERV_hpp
