/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jacopo <jacopo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 15:06:40 by jacopo            #+#    #+#             */
/*   Updated: 2025/03/25 14:56:17 by jacopo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#if !defined(WEBSERV_hpp)
#define WEBSERV_hpp


//include all the library here pls 

#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <string>
#include <map>


#include "CGIHandler.hpp"
#include "./Server.hpp" 
#include "./Location.hpp" 
#include "./ClientConnection.hpp"
#include "./Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "./HTTPServer.hpp"
//#include "Parser.hpp" //parser di ani
#include "Tokenizer.hpp"

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

void getServerByHost(const HTTPRequest& request, const Config & config);


#endif // WEBSERV_hpp
