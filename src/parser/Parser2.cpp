#include "webServ.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
/*

*/



int ParseWord(int look_for, std::string Word)
{ 
    if (look_for == IN_SERVER)
    {
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
        if (Word == "cgi_path")
		{
            return L_CGI_PATH_ARG;
		}
        if (Word == "cgi_extension")
		{
            return L_CGI_EXTENSION_ARG;
		}
        if (Word == "upload_store")
		{
            return L_UPLOAD_STORE_ARG;
		}
        if (Word == "client_max_body_size")
		{
            return L_CLIENT_MAX_BODY_SIZE;   
		}
    }
    return ERROR;//non ha trovato la parola che si aspettava
}


bool endsWithSemicolon(const std::string& word)
{
    if (word.empty()) 
	{
        return false; //torn
    }
    return (word[word.size() - 1] == ';');
}


/*
mette nei rispettivi vettori dei field gli argomenti.
controlla se ci sono troppi argomenti in base al field 
(ad esempio index puo averne infiniti, root solo uno)
semicolon definisce se ha smesso di inserire argomenti o no.
per controllare il numero confronta il numero di argomenti nel 
vettore con l'ultima cofra del "look_for" attuale
nell'ultima cifra del look_for attuale c'è scritto quanti argomenti puo accettare 
quel field ad esempio root finisce con 1, error page finisce con 2
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
			if (!((static_cast<int>(args.size())) == look_for % 10 || look_for == INDEX_ARG))
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

/*
stessa cosa di insertArgInField ma per i Metodi, controlla che non ci siano ripetizione e che siano i tre concessi
(ancora da testare)
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
				<< ": are your methods wright? unexpexted token: '" << Word << "'\n";//trovata parola errata
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


/*
lo stream iss controlla per ogni linea tutte le parole.
la funzione va per stati definiti in look_for:
il valore di look_for (sono macro) 
definisce COSA sta cercndo la funzione,
se trova quello che cerca va avanti, altrimenti da errore 
man mano che trova riempe un vettore di istanze di server che 
vengono progressivamente compilate. 
se trova parole che non si aspetta va in errore, controlla che ci siano almeno root server_name e port per chiudere un server,
controlla non ci siano ripetizioni di campi.
*/
int ParseFileLineByLine(const std::string& filePath, std::vector<Server>& servers)
{
    int look_for = SERVER;
    std::ifstream file(filePath.c_str());
    if (!file.is_open())
    {
        std::cerr << "Unable to open file: " << filePath << std::endl;
        return -1;
    }
    std::string line;
    int n_line = 0;
    while (std::getline(file, line) && ++n_line)
    {
        std::istringstream iss(line);
        std::string Word;
        
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
                    		std::cout << "trovato '{'\n";
                		}
                		break;
            		}
            		case IN_SERVER:
            		{
						if (Word == "}") //se trova brackets controlla che ci siano i 3 obbligatori, se non li trova va in errore
						{
							std::cout << "trovato '}'\n";
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
						}
                		else if ((look_for = ParseWord(look_for, Word))) //cambia look for in base a cio che trova, se trova 0 è errore
                		{
                    		if (!servers.back().setNumberToZero(look_for)) //se trova due volte lo stesso campo va in errore
							{
								look_for = ERROR;
								std::cout << "syntax error at line " << n_line 
								<< ": watch out for '" << Word 
								<< "' repetition in your conf\n";
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
						if (insertArgInField(Word, look_for, servers.back().listen, n_line))
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
						if (insertArgInField(Word, look_for, servers.back().index, n_line))
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
						servers.back().error_page.push_back(std::vector<std::string>());
						if (insertArgInField(Word, look_for, servers.back().error_page.back(), n_line))
						{
							if (endsWithSemicolon(Word))
								look_for = IN_SERVER;
						}
						else 
							look_for = ERROR;
						break;
					}
					case CLIENT_MAX_BODY_SIZE:
					{
						if (insertArgInField(Word, look_for, servers.back().client_max_body_size,  n_line))
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
                    		std::cout << "trovato '{'\n";
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
						servers.back().location.push_back(Location());
						if (insertArgInField(Word, look_for, servers.back().location.back().path, n_line))
						{
								look_for = OPEN_L_BRACKET;
						}
						else 
							look_for = ERROR;
						break;
					}
					case IN_LOCATION:
					{
						if (Word == "}") //se trova brackets controlla che ci siano i 3 obbligatori, se non li trova va in errore
						{
							std::cout << "trovato '}'\n";
							look_for = IN_SERVER;
							break;
						}
                		else if ((look_for = ParseWord(look_for, Word))) //cambia look for in base a cio che trova, se trova 0 è errore
                		{
                    		if (!servers.back().location.back().setNumberToZero(look_for)) //se trova due volte lo stesso campo va in errore
							{
								look_for = ERROR;
								std::cout << "syntax error at line " << n_line 
								<< ": watch out for '" << Word 
								<< "' repetition in your conf\n";
								break;
							}
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
						if (insertArgInField(Word, look_for, servers.back().location.back().l_cgi_extension, n_line))
						{
							if (endsWithSemicolon(Word))
								look_for = IN_LOCATION;
						}
						else 
							look_for = ERROR;
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
					case L_INDEX_ARG:
					{
						if (insertArgInField(Word, look_for, servers.back().location.back().l_index, n_line))
						{
							if (endsWithSemicolon(Word))
								look_for = IN_LOCATION;
						}
						else 
							look_for = ERROR;
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
						if (insertArgInField(Word, look_for, servers.back().location.back().l_cgi_path, n_line))
						{
							if (endsWithSemicolon(Word))
								look_for = IN_LOCATION;
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
					case L_CLIENT_MAX_BODY_SIZE:
					{
						if (insertArgInField(Word, look_for, servers.back().location.back().l_client_max_body_size, n_line))
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
			std::cout << "look for =" << look_for << "sono uscito dalo switch\n";
			std::cout << Word << " è la parola con cui sono uscito\n";
        }
    }
	//funzione che controlli non ci siano ripetizioni di server name
	//funzione che controlli non ci siano ripetizioni di port nei vari server
	//funzione che controlli non ci siano ripetizioni di path per le location di ogni server(?)
	//funzione che controlli le error page siano corrette;
	//eventualmente inserendo questi controlli nello switch case potremmo dire all'utente a che riga sta l'errore

    file.close();
    return 1;
}