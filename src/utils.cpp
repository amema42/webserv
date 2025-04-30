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

std::string generateUniqueFilename(std::string suffix) {
    std::time_t now = std::time(0);
    std::tm* timeinfo = std::localtime(&now);
    std::ostringstream oss;
    oss << "upload_"
        << (timeinfo->tm_year + 1900)  
        << (timeinfo->tm_mon + 1)      
        << timeinfo->tm_mday           
        << "_"
        << timeinfo->tm_hour          
        << timeinfo->tm_min            
        << timeinfo->tm_sec            
        << "." + suffix;
    return oss.str();
}

std::string handleMultipartBody(std::string body) {
    size_t firstLineEnd = body.find("\r\n");
    if (firstLineEnd == std::string::npos) {
        return "";
    }
    //potrebbe non servire
    std::string boundary = body.substr(0, firstLineEnd);
    size_t headerEnd = body.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        return ""; // Header malformati
    }
    size_t contentStart = headerEnd + 4; 
    size_t contentEnd = body.find("\r\n" + boundary, contentStart);
    if (contentEnd == std::string::npos) {
        return body.substr(contentStart);
    }
    return body.substr(contentStart, contentEnd - contentStart);
}

//in caso si potrebbe creare una classe se serviranno più dati dall'header del post
//non deve essere const
std::string CreateFileName(HTTPRequest& request){
    std::string rawType = getHeaderValue("Content-Type", request);
    std::string filename;
    std::istringstream stream(rawType);
    std::string type;
    stream >>  type;
    if(type == "multipart/form-data;"){
         std::string rawDisposition = request.body.substr(request.body.find("filename=")+ 10);
         filename = rawDisposition.substr(0, rawDisposition.find("\""));
         request.body = handleMultipartBody(request.body);
         return filename;
    }
    std::string suffix = type.substr(type.find("/") + 1);
    filename = generateUniqueFilename(suffix);
    return filename;
}


Location&  getLocationByName(std::string path, Server& server){
    for(size_t i = 0; i < server.location.size(); i++){
        if(server.location[i].path[0] == path){
            return server.location[i];
        }
    }
    throw std::runtime_error("no location founded");
}


bool methodIsAllowed(std::string uri, std::string method, Server& server){
    if (uri == "/")
        return true;
    else{
        if (uri[uri.size()-1] == '/'){
            uri = uri.substr(0, uri.size() - 1);
        }
        try{
            Location& location = getLocationByName(uri, server);
            if(location.l_methods.size() == 0)
            return true;
            for (size_t i = 0; i < location.l_methods.size(); i++){
                if (location.l_methods[i] == method)
                return true;
            }
        }
        catch (std::exception& e){
            return true;
        }
        
    }
    return false;
}


