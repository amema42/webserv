#include "webServ.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>

int ParseWord(int look_for, std::string Word)
{
    if (look_for == IN_SERVER)
    {
		 if (Word == "cgi_path")
		{
            return L_CGI_PATH_ARG;
		}
        if (Word == "cgi_extension")
		{
            return L_CGI_EXTENSION_ARG;
		}
        if (Word == "listen")
		{
            return LISTEN_ARG;
		}
        if (Word == "server_name")
		{
            return SERVER_NAME_ARG;
		}
        if (Word == "root")
		{
            return ROOT_ARG;
		}
        if (Word == "index")
		{
            return INDEX_ARG;
		}
        if (Word == "error_page")
		{
			return ERROR_PAGE_ARG;
		}
        if (Word == "client_max_body_size")
		{

			return CLIENT_MAX_BODY_SIZE;
		}
        if (Word == "location")
		{
            return LOCATION_PATH;
		}
    }
    else if (look_for == IN_LOCATION)
    {
		if (Word == "autoindex")
		{
            return AUTOINDEX;
		}
		if (Word == "return")
		{
            return REDIRECTION;
		}
        if (Word == "root")
		{
            return L_ROOT_ARG;
		}
        if (Word == "index")
		{
            return L_INDEX_ARG;
		}
        if (Word == "methods")
		{
            return L_METODS_ARG;
		}
        if (Word == "upload_store")
		{
            return L_UPLOAD_STORE_ARG;
		}
    }
    return ERROR;//non ha trovato la parola che si aspettava 
}


bool endsWithSemicolon(const std::string& word)
{
    if (word.empty())
	{
        return false;
    }
    return (word[word.size() - 1] == ';');
}


/*
This function inserts arguments into the respective field vectors based on the current context (`look_for`).
It validates the number of arguments allowed for each field by comparing the size of the vector with the last digit of the `look_for` value.
For example:
- `root` can accept only one argument (last digit is 1).
- `error_page` can accept two arguments (last digit is 2).
- `index` can accept an unlimited number of arguments (last digit is 9).

The presence of a semicolon (`;`) determines whether argument insertion has concluded for the current field.
If the number of arguments exceeds the allowed limit or other syntax issues are detected, the function reports an error.
*/
bool insertArgInField(std::string& Word, int look_for, std::vector<std::string>& args, int n_line)
{
	if (((static_cast<int>(args.size())) < (look_for % 10)) || ((look_for % 10) == 9))
	{
		//std::cout << ((static_cast<int>(args.size())) < (look_for % 10)) << "------\n" ;
		if (endsWithSemicolon(Word))
		{
			args.push_back(Word.substr(0, Word.size() - 1));
			//int pop = (look_for % 10 );
			if ((static_cast<int>(args.size())) != look_for % 10)
			{
				std::cout << "syntax error at line " << n_line << "incorrect number of arguments at token: '" << Word << "'\n";
				return false;
			}
			return true;
		}
		args.push_back(Word);

		return true;
	}
	std::cout << "syntax error at line " << n_line << "incorrect number of arguments at token: '" << Word << "'\n";
	return false;
}




bool insertArgInIndex(std::string& Word, std::vector<std::string>& args)
{
		std::cout << Word << "\n" ;
		if (endsWithSemicolon(Word))
		{
			args.push_back(Word.substr(0, Word.size() - 1));
			//int pop = (look_for % 10 );
			return true;
		}
		else
			args.push_back(Word);

		return true;
}

/*
Similar to insertArgInField, but specifically for HTTP methods. 
This function ensures that there are no duplicate methods and only the three allowed methods 
(GET, POST, DELETE) are accepted. 
(Still needs testing)
*/
bool insertInMethods(std::istringstream& iss, std::string& Word, int look_for, Location& location, int n_line)
{
	if (look_for == L_METODS_ARG)
	{
		bool get_m = false;
		bool post_m = false;
		bool delete_m = false;
		while (!endsWithSemicolon(Word) && Word != "")
		{
			if (Word == "GET" && !get_m)
				get_m = true;
			else if (Word == "POST" && !post_m)
				post_m = true;
			else if (Word == "DELETE" && !delete_m)
				delete_m = true;
			else
			{
				std::cout << "syntax error at line " << n_line
				<< ": are your methods wright? unexpexted token: '" << Word << "'\n"; //trovata parola errata
				return false;
			}
			location.addToLMethods(Word);
			iss >> Word;

		}
		if (Word == "GET;" && !get_m)
			get_m = true;
		else if (Word == "POST;" && !post_m)
			post_m = true;
		else if (Word == "DELETE;" && !delete_m)
			delete_m = true;
		else
		{
			std::cout << "syntax error at line " << n_line
			<< ": are your methods wright? unexpexted token: '" << Word << "'\n";
			if (Word == "")
				std::cout << "';' not found \n";
			return false;
		}
		location.addToLMethods(Word.substr(0, Word.size() - 1));
		return true;
	}
	std::cout << "syntax error at line " << n_line
	<< "incorrect number of arguments at token: '" << Word << "'\n";
	return false;

}

size_t max_body_detection(const std::string& word)
{
	std::string sizeStr = word.substr(0, word.size() - 1);

	// Controllo se termina con "K" o "M" (case sensitive)
	size_t multiplier = 1;
	char lastChar = sizeStr[sizeStr.size() - 1];

	if (lastChar == 'K')
	{
		multiplier = 1024;
		sizeStr = sizeStr.substr(0, sizeStr.size() - 1);
	}
	else if (lastChar == 'M')
	{
		multiplier = 1024 * 1024;
		sizeStr = sizeStr.substr(0, sizeStr.size() - 1);
	}
	else if (!isdigit(lastChar)) // fino qua vede se ci sono K, M o nulla alla fine se trova altro muore
	{
		std::cout << "watch out! only M or K or blank (ex 1000) accepted: ";
		return static_cast<size_t>(0);
	}
	// vede se la stringa contiene solo cifre
	for (size_t i = 0; i < sizeStr.size(); ++i)
	{
		if (!isdigit(sizeStr[i]))
		{
			std::cout << "watch out for digits!: ";
			return static_cast<size_t>(0);
		}
	}
	long int converted = atoi(sizeStr.c_str());
	if (converted <= 0 || static_cast<size_t>(converted) > SIZE_MAX / multiplier)
	{
		std::cout << "invalid  value!: ";
		return static_cast<size_t>(0);
	}
	return static_cast<size_t>(converted) * multiplier;
}

bool insertArgInMax(std::string& Word, int look_for, std::vector<size_t>& args, int n_line)
{
	if (((static_cast<int>(args.size())) < (look_for % 10)) || ((look_for % 10) == 9))
	{
		//std::cout << ((static_cast<int>(args.size())) < (look_for % 10)) << "------\n" ;
		if (endsWithSemicolon(Word))
		{
			if (max_body_detection(Word) > 0)
			{
				args.push_back(max_body_detection(Word));
			}
			else
			{
				std::cout << "error at line "<< n_line << std::endl;
				return false;
			}
			//args.push_back(static_cast<size_t>(std::atoi(Word.substr(0, Word.size() - 2).c_str())));
			//int pop = (look_for % 10 );
			if (!((static_cast<int>(args.size())) == look_for % 10 || look_for == INDEX_ARG))
			{
				std::cout << "syntax error at line " << n_line << "incorrect number of arguments at token: '" << Word << "'\n";		
				return false;
			}
			return true;
		}
		return true;
	}
	std::cout << "syntax error at line " << n_line << "incorrect number of arguments at token: '" << Word << "'\n";	
	return false;
}



bool isNameValid(const std::string& Word, const std::vector<Server>& server)
{
	if (server.size() <= 1)
	{
		//std::cout << "entrato nel controllo giusto\n";
		return true;
	}
	int i = 0;
	while (i < static_cast<int>(server.size()) - 1 && server[i].server_name.back() != Word.substr(0, Word.size() - 1))
		i++;
	if (i == static_cast<int>(server.size()) - 1)
		return true;
	else 
		return false;
	return true;
}

bool isPortValid(const std::string& Word, const std::vector<Server>& server)
{
	if (server.size() <= 1)
	{
		//std::cout << "entrato nel controllo giusto--------------------------------------\n";
		return true;
	}
	int i = 0;
	while (i < static_cast<int>(server.size()) -1  && server[i].listen.back() != static_cast<int>(std::atoi(Word.substr(0, Word.size() - 1).c_str())))
		i++;
	if (i == static_cast<int>(server.size()) - 1)
		return true;
	else 
		return false;
	return true;
}

bool insertArgInListen(std::string& Word, int look_for, std::vector<int>& args, int n_line)
{
	if (((static_cast<int>(args.size())) < (look_for % 10)) || ((look_for % 10) == 9))
	{
		//std::cout << ((static_cast<int>(args.size())) < (look_for % 10)) << "------\n" ;
		if (endsWithSemicolon(Word))
		{
			args.push_back(static_cast<int>(std::atoi(Word.substr(0, Word.size() - 1).c_str())));
			//int pop = (look_for % 10 );
			if ((static_cast<int>(args.size())) != look_for % 10)
			{
				std::cout << "syntax error at line " << n_line << "incorrect number of arguments at token: '" << Word << "'\n";		
				return false;
			}
			return true;
		}
		return true;
	}
	std::cout << "syntax error at line " << n_line << "incorrect number of arguments at token: '" << Word << "'\n";	
	return false;
}

bool isWordValid(const std::string& Word, const std::vector<Location>& locations)
{
    size_t i = 0;
    // Scorri ciascuna Location con while
    while (i < locations.size()) {
        // Ottieni il vettore di path per questa location
        const std::vector<std::string>& paths = locations[i].path;
        // Se non è .empty + l'ultimo elemento è uguale a Word => ripetuto
        if (!paths.empty() && paths.back() == Word) {
            return false;
        }
        ++i;
    }
    // Se non l'abbiamo mai trovato, è isValid
    return true;
}

bool endsWithPython(const std::string& str) 
{
    const std::string suffix = ".py;";
    
    // Se la stringa è più corta del suffisso, non può terminare con esso
    if (str.size() < suffix.size()) 
        return false;
    
    // Confronta la parte finale della stringa con il suffisso
    return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}


bool keyExists(std::map<std::string, std::string>& myMap, const std::string& key)
{
    return myMap.find(key) != myMap.end();
}


//controlla che il codice inserito come error page sia giusto
bool check_error_code(std::map<std::string, std::string>& error_map, const std::string& error_code)
{
	if (keyExists(error_map, error_code))
	{
		std::cout << "you already have this error code: '" << error_code << "' defined! ";
		return false;
	}
	if (!(error_code == "400" || error_code == "403" || error_code == "404" || error_code == "405" || error_code == "413" || error_code == "500"))
	{
		std::cout << "unexpected error code: '" << error_code << "' ";
		return false;
	}
	return true;
}

//controlla che il codice inserito di redirect sia giusto e che non ci siano due codici
bool check_redirect_code(std::vector<std::string>& redirect, const std::string& redirect_code)
{
	if (!redirect.empty())
	{
		std::cout << "you already have a redirection in this location: '" << redirect_code << "' is one too many ";
		return false;
	}
	if (!(redirect_code == "301" || redirect_code == "302" ))
	{
		std::cout << "unexpected redirect code: '" << redirect_code << "' ";
		return false;
	}
	return true;
}

/*
The `iss` stream processes each line word by word.
The function operates based on states defined by the `look_for` variable:
- The value of `look_for` (defined as macros) determines what the function is currently expecting.
- If the expected token is found, the function progresses; otherwise, it reports an error.

As the function parses the configuration file, it populates a vector of `Server` instances, progressively filling in their fields.
The function ensures:
- At least `root`, `server_name`, and `listen` fields are defined before closing a server block.
- No duplicate fields are present within a server or location block.
- Syntax errors, such as unexpected tokens or missing semicolons, are detected and reported.

If unexpected tokens are encountered or required fields are missing, the function transitions to an error state and terminates parsing.
*/
int ParseFileLineByLine(const std::string& filePath, std::vector<Server>& servers)
{
    int look_for = SERVER;
    std::ifstream file(filePath.c_str());
    if (!file.is_open())
    {
        std::cerr << "Unable to open file: " << filePath << std::endl;
        return 0;
    }
    std::string line;
    int n_line = 0;
    while (std::getline(file, line) && ++n_line)
    {

        std::istringstream iss(line);
        std::string Word;
		size_t i = 0;
    	while (i < line.length() && std::isspace(line[i])) 
		{
       		++i;
		}
	if ((line.c_str())[i] != '#')
	{
        while (iss >> Word)
		{

				if (Word == ";")
				{
					look_for = ERROR;
					std::cout << "syntax error at line " << n_line
					<< ": found space before ';'\n";
					break;
				}

            	switch (look_for)
				{
            		case SERVER:
            		{
                		if (Word == "server" )
               			{
                  			look_for = OPEN_S_BRACKET;
                			std::cout << "trovato server\n";
                		}
						else
						{
							look_for = ERROR;
							std::cout << "syntax error at line " << n_line
							<< ": unexpected token: '" << Word
							<< "' instead of 'server' \n";
						}
						//std::cout << "sto per uscire\n";
                		break;
            		}
            		case OPEN_S_BRACKET:
            		{
                		if (Word == "{" )
                		{
                    		look_for = IN_SERVER;
							servers.push_back(Server()); // server creato
                    		//std::cout << "trovato '{'\n";
                		}
                		break;
            		}
            		case IN_SERVER:
            		{
						if (Word == "}") //se trova brackets controlla che ci siano i 3 obbligatori, se non li trova va in errore
						{
							//std::cout << "trovato '}'\n";
							if (!servers.back().setNumberToZero(LISTEN_ARG)
								&& !servers.back().setNumberToZero(SERVER_NAME_ARG)
								&& !servers.back().setNumberToZero(ROOT_ARG))
								look_for = SERVER;
							else
							{
								look_for = ERROR;
								std::cout << "syntax error at line " << n_line
								<< ": watch out for '" << Word
								<< "' closed brackets without 'listen', 'server_name' or 'root' field\n";
							}
							if (servers.back().client_max_body_size.size() == 0)
								servers.back().client_max_body_size.push_back(1048576);

						}
                		else if ((look_for = ParseWord(look_for, Word))) //cambia look for in base a cio che trova, se trova 0 è errore
                		{
                    		if (!servers.back().setNumberToZero(look_for)) //se trova due volte lo stesso campo va in errore
							{
								look_for = ERROR;
								std::cout << "syntax error at line " << n_line
								<< ": watch out for '" << Word
								<< "' repetition in your confo\n";
							}
							
                		}
						else
						{
							std::cout << "syntax error at line " << n_line
							<< ": unexpected token '" << Word << "' \n";
						}
						break;
					}
					case LISTEN_ARG:
					{
						if (!isPortValid(Word, servers))
						{
							std::cout << "syntax error at line " << n_line
							<< ": port n: '" << Word << "' already used!\n";
							look_for = ERROR;
							break;
						}
						if (insertArgInListen(Word, look_for, servers.back().listen, n_line))
						{
							if (endsWithSemicolon(Word))
								look_for = IN_SERVER;
						}
						else
							look_for = ERROR;
						break;
					}
					case SERVER_NAME_ARG:
					{
						if (!isNameValid(Word, servers))
						{
							std::cout << "syntax error at line " << n_line
							<< ": name '" << Word << "' already used!\n";
							look_for = ERROR;
							break;
						}
						if (insertArgInField(Word, look_for, servers.back().server_name, n_line))
						{
							if (endsWithSemicolon(Word))
								look_for = IN_SERVER;
						}
						else
							look_for = ERROR;
						break;
					}
					case ROOT_ARG:
					{
						//controllo che sia non utilizzata da altri serv
						if (insertArgInField(Word, look_for, servers.back().root, n_line))
						{
							if (endsWithSemicolon(Word))
								look_for = IN_SERVER;
						}
						else
							look_for = ERROR;
						break;
					}
					case INDEX_ARG:
					{
						if (insertArgInIndex(Word, servers.back().index))
						{
							if (endsWithSemicolon(Word))
								look_for = IN_SERVER;
						}
						else
							look_for = ERROR;
						break;
					}
					case ERROR_PAGE_ARG:
					{	
						std::string error_code = Word;
    					std::string error_page;
						if(!check_error_code(servers.back().error_page, error_code))  //-controlla che ci sia l'error giusto e che non sia gia stato fatto
						{
							std::cout << " syntax error at line " << n_line << std::endl;
							look_for = ERROR;
							break;
						}
   						if (!(iss >> error_page))
						{
        					std::cout << "syntax error: missing error page for code " << error_code << "\n";
        					look_for = ERROR;
        					break;
    					}
    					if (endsWithSemicolon(error_page))
						{
        					error_page = error_page.substr(0, error_page.size() - 1);
        					look_for = IN_SERVER;
    					}
						else
						{
							look_for = ERROR;
							std::cout << "syntax error: no closed semicolon at line " << n_line << "\n";
							break;
						}
						servers.back().error_page[error_code] = error_page;
						break;
					}
					case CLIENT_MAX_BODY_SIZE:
					{
						if (insertArgInMax(Word, look_for, servers.back().client_max_body_size,  n_line))
						{
							if (endsWithSemicolon(Word))
								look_for = IN_SERVER;
						}
						else
							look_for = ERROR;
						break;
					}
					case OPEN_L_BRACKET:
					{
						if (Word == "{" )
                		{
                    		look_for = IN_LOCATION;
                    		//std::cout << "trovato '{'\n";
                		}
						else
						{
							std::cout << "syntax error at line " << n_line
							<< ": unexpected token '" << Word << "' instead of '{' \n";
							look_for = ERROR;
						}
                		break;
					}
					case LOCATION_PATH:
					{
						if (!isWordValid(Word, servers.back().location))
						{
							std::cout << "syntax error at line " << n_line
							<< ": path '" << Word << "' is already taken in this server";
							look_for = ERROR;
							break;
						}
						servers.back().location.push_back(Location());
						servers.back().location.back().path.push_back(Word);
						look_for = OPEN_L_BRACKET;
						break;
					}
					case IN_LOCATION:
					{
						if (Word == "}") //se trova brackets controlla che ci siano i 3 obbligatori, se non li trova va in errore
						{
							look_for = IN_SERVER;

							//std::cout << "trovato '}'\n";
							break;
						}
                		else if ((look_for = ParseWord(look_for, Word))) //cambia look for in base a cio che trova, se trova 0 è errore
                		{
							// if (!servers.back().location.back().setNumberToZero(look_for)) //se trova due volte lo stesso campo va in errore
							// {
							// 	look_for = ERROR;
							// 	std::cout << "syntax error at line " << n_line
							// 	<< ": watch out for '" << Word
							// 	<< "' repetition in your conf\n";
							// 	break;
							// }
                    		break;
                		}
						else
						{
							std::cout << "syntax error at line " << n_line
							<< ": unexpected token '" << Word << "' \n";
							break;
						}
					}
					case L_CGI_EXTENSION_ARG:
					{
						if (endsWithPython(Word) && insertArgInField(Word, look_for, servers.back().l_cgi_extension, n_line))
						{
								look_for = IN_SERVER;
						}
						else
						{
							std::cout << "syntax error at line: " << n_line << ", is your cgi extention ok?\n";
							look_for = ERROR;
						}
						break;
					}
					case L_ROOT_ARG:
					{

						if (insertArgInField(Word, look_for, servers.back().location.back().l_root, n_line))
						{
							if (endsWithSemicolon(Word))
								look_for = IN_LOCATION;
						}
						else
							look_for = ERROR;
						break;
					}
					case REDIRECTION:
					{	
						std::string redirection_code = Word;
    					std::string redirection_page;
						if(!check_redirect_code(servers.back().location.back().redirect_page, redirection_code))  
						{
							std::cout << " syntax error at line " << n_line << std::endl;
							look_for = ERROR;
							break;
						}
   						if (!(iss >> redirection_page))
						{
        					std::cout << "syntax error: missing redirection page for code " << redirection_code << "\n";
        					look_for = ERROR;
        					break;
    					}
    					if (endsWithSemicolon(redirection_page))
						{
        					redirection_page = redirection_page.substr(0, redirection_page.size() - 1);
        					look_for = IN_LOCATION;
    					}
						else
						{
							look_for = ERROR;
							std::cout << "syntax error: no closed semicolon at line " << n_line << "\n";
							break;
						}
						servers.back().location.back().redirect_page.push_back(redirection_code);
						servers.back().location.back().redirect_page.push_back(redirection_page);
						break;
					}
					case AUTOINDEX:
					{
						if (servers.back().location.back().autoindex.length() != 0)
						{
							look_for = ERROR;
								std::cout << "word = " << Word;
							std::cout << " syntax error: double autoindex in location at line " << n_line << "\n";
						}
						if (Word.substr(0, Word.size() - 1) != "on" && Word.substr(0, Word.size() - 1) != "off")
						{
							look_for = ERROR;
							std::cout << "syntax error: at line " << n_line << " only 'on' or 'off' accepted for autoindex field\n";
						}
						if (endsWithSemicolon(Word))
        					look_for = IN_LOCATION;
						else
						{
							look_for = ERROR;
							std::cout << "syntax error: no closed semicolon at line " << n_line << "\n";
							break;
						}
						servers.back().location.back().autoindex = Word.substr(0, Word.size() - 1);
						if (servers.back().location.back().autoindex == "on")
							servers.back().location.back().autoindex_flag = true;
						break;
					}
					case L_INDEX_ARG:
					{
						if (insertArgInIndex(Word, servers.back().location.back().l_index))
						{
								look_for = IN_LOCATION;
						}
						else
						{
							look_for = ERROR;
						}
						break;
					}
					case L_METODS_ARG:
					{
						if (insertInMethods(iss, Word, look_for, servers.back().location.back(), n_line))
						{
								look_for = IN_LOCATION;
						}
						else
							look_for = ERROR;
						break;
					}
					case L_CGI_PATH_ARG:
					{
						if (insertArgInField(Word, look_for, servers.back().l_cgi_path, n_line))
						{
							if (endsWithSemicolon(Word))
								look_for = IN_SERVER;
						}
						else
							look_for = ERROR;
						break;
					}
					case L_UPLOAD_STORE_ARG:
					{
						if (insertArgInField(Word, look_for, servers.back().location.back().l_upload_store, n_line))
						{
							if (endsWithSemicolon(Word))
								look_for = IN_LOCATION;
						}
						else
							look_for = ERROR;
						break;
					}
            		case ERROR:
                		file.close();
                		return 0;
    		}
        }
		}
    }

    file.close();
    return 1;
}