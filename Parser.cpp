#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

/*
allora il ragionamento voleva essere fare un grosso switch case
per creare direttamente le classi server e location e iniziare a dare eventuali
syntax error mentre tokenizzavo e conservavo le parole, ma me sa è stata na bella cazzata
perché ho basato tutto sul gatto di poter avere massimo tre parole su una riga, std::istringstream iss(line);permette di storare le parole saltandotab e spazi.
comunque level 1 è quando cerco il la parola server, level 2 cerco {
level 3 cerco una delle parole chiavi che compilano gli attributi del server
level 41 (sarebbe 4-1) siamo entrati dentro location e ci aspettiamo di nuovo {
incontrare } fa tornare al livello 3
incontrare } al livello 3 fa tornare al livello 1
devo capire meglio tutte le parole chiave. non sto cancellando ancora nulla
ma apparte le ultime 2 funzioni il resto è cacca 
*/

std::string FileInString(std::string path)
{
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return (buffer.str());
}

bool ContainsSubstringAt(const char* str, const std::string& substr)
{
    return std::string(str).find(substr) == 0;
}

void IterateAndCheckSubstring(const std::string& str, const std::string& substr)
{
    const char* cstr = str.c_str();
    while (*cstr)
    {
        if (ContainsSubstringAt(cstr, substr))
        {
            cstr + sizeof(substr);
            std::cout << "Found substring at position: " << (cstr - str.c_str()) << std::endl;
        }
        if (*cstr == 32 )
        ++cstr;
    }
}

int ParseFirstWord(int level, std::string firstWord)
{
    if (level == 3)
    {
        if (firstWord == "listen")
            return 1;
        if (firstWord == "server_name")
            return 2;
        if (firstWord == "root")
            return 3;
        if (firstWord == "index")
            return 4;
        if (firstWord == "error_page")
            return 5;
        if (firstWord == "client_max_body_size")
            return 6;
        if (firstWord == "location")
            return 6;
    }
    else if (level = 41)
    {
        if (firstWord == "root")
            return 8;
        if (firstWord == "index")
            return 9;
        if (firstWord == "methods")
            return 10;
        if (firstWord == "cgi_path")
            return 12;
        if (firstWord == "cgi_extension")
            return 13;
        if (firstWord == "upload_store")
            return 14;
        if (firstWord == "client_max_body_size")
            return 15;   
    }
    return 0;
}

int ParseFileLineByLine(const std::string& filePath)
{
    int level = 1;
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Unable to open file: " << filePath << std::endl;
        return;
    }

    std::string line;
    int x = 1;
    while (std::getline(file, line) && x++)
    {
        std::istringstream iss(line);
        std::string firstWord;
        std::string secondWord;
        std::string thirdWord;
        iss >> firstWord >> secondWord >> thirdWord;
        while (1)
        {
            if (firstWord == "" || "#")
                break;
           switch (level)
           {
            case 1:
            {
                if (firstWord == "server" && secondWord == "")
                {
                    level = 2;//level 2 cerca {
                    //crea un server
                    std::cout << "trovato server\n";
                }
                else if (firstWord == "server" && secondWord == "{" && thirdWord == "")
                 {
                    level = 3;//level 2 cerca {
                    //crea un server
                    std::cout << "trovato server e brackets\n";
                }
                else
                {
                    std::cout << "syntax error at line " << x << ", first line should contain only 'server' \n";
                    level = 0;
                }
                break;
            }
            case 2:
            {
                if (firstWord == "{" && secondWord == "")
                {
                    level = 3;
                    std::cout << "trovato '{'\n";
                }
                else
                    std::cout << "syntax errorat line " << x << ", expected '{' and newline to open context\n";
                break;
            }
            case 3:
            {
                 if (secondWord == "")
                 {
                    std::cout << "syntax error at line " << x << ", absent value \n";
                    level = 0;
                    break;
                 }
                if (ParseFirstWord(level, firstWord))
                {
                    if (ParseFirstWord(level, firstWord) == 6)
                    {
                        level = 41;
                        //store second word
                        std::cout << "trovato location\n";
                    }
                    else if (ParseFirstWord(level, firstWord) == 5)
                    {
                        if (thirdWord == "")
                        {
                            std::cout << "syntax error at line " << x << ", absent value \n";
                            level = 0;
                        }
                        level = 3;
                        //store second word 
                        //store third word 
                        std::cout << "trovata error page\n";
                    }
                    else
                    {
                        level = 3;
                        std::cout << "trovata prola chiave di server\n";
                    }

                }
                else
                    std::cout << "syntax error, expected '{' and newline to open context\n";
                break;
                }
            case 0:
                file.close();
                return 0;
           }
        }
    }   

    file.close();
    return 1;
}