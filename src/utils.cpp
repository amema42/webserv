#include "webServ.hpp"

std::string readFile(const std::string &filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        throw std::runtime_error("Impossibile aprire il file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string getHeaderValue(const std::string toFind, const HTTPRequest& request){
    std::map<std::string, std::string>::const_iterator it = request.headers.find(toFind);
    if ( it != request.headers.end())
        return it->second;
    else{
        std::cout << "no value can be found" << std::endl;
        return "";
    }
}

Server& getServerByHost(const HTTPRequest& request, const Config & config){
    std::string Host = getHeaderValue("Host", request);
    std::size_t pos = Host.find(":");
    int port = atoi((Host.substr(pos + 1)).c_str());
    std::cout << "valore trovato dalla funzione getServerByHost: "<< port << std::endl;
    size_t i = 0;
    while(i < (*config.servers).size()){
         if((*config.servers)[i].listen[0] == port){
            return ((*config.servers)[i]);
             // std::cout << " ho trovato il server" << (*config.servers)[i].server_name[0] << std::endl;
         }
        i++;
    }
    throw std::exception();
}


void printMap(std::map<std::string, std::string>& map, std::string first, std::string second){

    std::map<std::string, std::string>::iterator it = map.begin();
    while(it != map.end()){
        std::cout << first <<it->first << second << it->second << std::endl;
        it++;
    }
}

//in caso si potrebbe creare una classe se serviranno più dati dall'header del post
std::string CreateFileName(const HTTPRequest& request){
    std::map<std::string, std::string> map = request.headers;
    printMap(map, "1", " 2");
    std::cout <<"IL BODY"<< request.body << std::endl;
    
    std::string rawType = getHeaderValue("Content-Type", request);
    std::string filename;
    std::istringstream stream(rawType);
    std::string type;
    stream >>  type;
    std::cout << "content type = " << type << std::endl;
    if(type == "multipart/form-data;"){
        std::cout << "aaaaaaaaaaaaaa" <<std::endl;
        std::string rawDisposition = getHeaderValue("Content-Disposition",request);
        std:: istringstream stream2(rawDisposition);
        std::string tmp;
        while(stream2 >> tmp)
            filename = tmp;
    }
    std::cout<<"il file name  è: "<< filename<<std::endl;
    return "tmp";
}


