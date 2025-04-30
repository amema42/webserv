#include "webServ.hpp"
#include "HTTPServer.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "ClientConnection.hpp"

HTTPServer::HTTPServer(const Config &config) : _config(config) {
	initSockets();
}

//necessita del perscorso file e di partire da zero, 
//restituisce una strnga html della struttura di cartelle
std::string HTTPServer::dirTree(const std::string& dirPath, int depth) //partire da 0
{
	DIR *dir;
	struct dirent *ent;
	std::stringstream html;

	if ((dir = opendir(dirPath.c_str())) != NULL)
	{
		if (depth == 0)
		{
			html << "<!DOCTYPE html>\n<html>\n<head>\n<title>Contenuto directory: " 
				 << dirPath << "</title>\n</head>\n<body>\n";
			html << "<h1>Contenuto directory: " << dirPath << "</h1>\n";
		}
		html << "<ul>\n";
		while ((ent = readdir(dir)) != NULL)
		{
			std::string name = ent->d_name;
			
			if (name != "." && name != "..") {
				std::string fullPath = dirPath + "/" + name;
				struct stat statbuf;
				
				if (stat(fullPath.c_str(), &statbuf) == 0)
				{
					if (S_ISDIR(statbuf.st_mode))
					{
						html << "<li><strong>" << name << "/</strong>\n";
						html << dirTree(fullPath, depth + 1);
						html << "</li>\n";
					} 
					else 
						html << "<li>" << name << "</li>\n";
				}
			}
		}
		html << "</ul>\n";
		closedir(dir);
		if (depth == 0) 
			html << "</body>\n</html>\n";
	} 
	else
		html << "<p>Impossibile aprire la directory</p>";
	return html.str();
}
// Distruttore: chiude i socket di ascolto
HTTPServer::~HTTPServer() {
    // close listen sockets
    for (size_t i = 0; i < _listenSockets.size(); ++i) { // ciclo sui listen socket
        close(_listenSockets[i]);
    }
}

// Funzione per inizializzare i socket in base ai server configurati
void HTTPServer::initSockets() {
    // inizializzazione dei socket
    for (size_t i = 0; i < (*_config.servers).size(); ++i) { // ciclo sui server configurati
        const Server &server = (*_config.servers)[i];

        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
            throw std::runtime_error("Error in socket creation.");
		 // Add this code to enable address reuse
		 int optval = 1;
		 if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) 
		 {
			close(sockfd);
			throw std::runtime_error("Error setting SO_REUSEADDR option.");
		 }
        int flags = fcntl(sockfd, F_GETFL, 0);
        if (flags < 0)
            throw std::runtime_error("Errore in fcntl F_GETFL");
        if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0)
            throw std::runtime_error("Errore in fcntl F_SETFL");

        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(server.listen[0]);
        addr.sin_addr.s_addr = INADDR_ANY;
        memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

        if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            close(sockfd);
            throw std::runtime_error("Error binding socket.");
        }

        if (listen(sockfd, 128) < 0) {
            close(sockfd);
            throw std::runtime_error("Error in listen().");
        }
        _listenSockets.push_back(sockfd);
        std::cout << "Server are listening on port " << server.listen[0] << std::endl;
    }
}

//controlla tutte le location e i rispettivi path ritorna true se autoindex_flag è true
bool HTTPServer::findLocationCheckAuto(Server& Server, std::string location)
{
	int i = 0;
	//std::cout << "----------------->debug: " << Server.location.size() << "\n";
	while (i < static_cast<int>(Server.location.size()))
	{
		//std::cout << "----------------->debug: " << Server.location[i].path[0] << "\n";
		if (Server.location[i].path[0] == location.substr(0, location.size() - 1))
			return (Server.location[i].autoindex_flag);
		i++;
	}
	return (false);
}


void HTTPServer::handleGetRequest(const HTTPRequest& request, HTTPResponse& response) {
    // seleziona il server in base all'host della richiesta
    Server& server = getServerByHost(request, _config);

    std::string fullpath = server.root[0] + request.uri;
    std::cout << "\t\t[GET] base path: " << fullpath << std::endl;

    struct stat st;
    // verifica se il path è una directory
    if (stat(fullpath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
        if (request.uri[request.uri.size() - 1] != '/')
        {
            response.setStatus(301, "Moved Permanently");
            response.setHeader("Location", request.uri + "/");
            return;
        }

        try {
            std::cout << "get location by name" << std::endl;
            Location& location = getLocationByName(request.uri.substr(0, (request.uri.size() -1)), server);
            fullpath += location.l_index[0];
        }
        catch(std::exception& e){
            std::cout << e.what() << std::endl;
            fullpath += server.index[0];
        }
        std::cout << "\t\t[GET] directory → path: " << fullpath << std::endl;
    }

    try {
        std::string content = readFile(fullpath);
        response.setStatus(200, "OK");
        response.body = content;
    }
    catch (std::exception& e)
    {
        std::cerr << "[GET] errore readFile: " << e.what() << std::endl;
        response.setStatus(404, "Not Found");
        try 
        {
			std::string errorPage = server.getErrorPage("404");
			//codice autoindex inizia qui
			if (findLocationCheckAuto(server, request.uri))
			{
				response.setStatus(200, "OK");
				response.body = dirTree(fullpath.substr(0, fullpath.size() - server.index[0].size() - 1), 0);
			}
			else
			{
				std::cout << "false, no autoindex\n";
				response.body = readFile(errorPage);
			}
			//codice autoindex finisce qui
        } catch (...) 
        {
            response.body = "<html><body><h1>404 Not Found</h1>"
                            "<p>File not found!.</p>"
                            "</body></html>";
        }
    }
}


void HTTPServer::handlePostRequest(HTTPRequest& request, HTTPResponse& response){
    
    // seleziona il server in base all'host della richiesta
    Server& server =  getServerByHost(request, _config);
    size_t i = std::strtol((getHeaderValue("Content-Length", request)).c_str(), NULL, 10);
    if(i > server.client_max_body_size[0]){
        try{
            std::string errorPath = server.getErrorPage("413");
            std::string errorContent = readFile(errorPath);
            response.body = errorContent;
        }
        catch(std::exception& e){
            std::cout << e.what() <<std::endl;
            response.body = "<html><body><h1>File Not Found</h1><p>default error page  error:413 a specific one are not provided in config file</p></body></html>";
        }
        return;
    }
    
    std::string fileName = CreateFileName(request);
    std::string uploadDir = server.root[0] + "/uploads/";
    struct stat st;
    // verifica se la directory uploads esiste, altrimenti la crea
    if (stat(uploadDir.c_str(), &st) == -1) {
        if (mkdir(uploadDir.c_str(), 0755) == -1) {
            response.setStatus(500, "Internal Server Error");
            try{
                std::string errorPath = server.getErrorPage("500");
                std::string errorContent = readFile(errorPath);
                response.body = errorContent;
            }
            catch(std::exception& e){
                std::cout << e.what() <<std::endl;
                response.body = "<html><body><h1>File Not Found</h1><p>default error page error:500 a specific one are not provided in config file</p></body></html>";
            }
            return;
        }
    }
    std::string fullpath = uploadDir + fileName;
    std::cout << fullpath << std::endl;
    std::cout << "server max body size: " << server.client_max_body_size[0]<< " Content size" << i << std::endl;
    try {
        std::ofstream outFile(fullpath.c_str(), std::ios::binary);
        outFile.write(request.body.data(), request.body.size());
        
        response.setStatus(201, "Created");
        response.setHeader("Location", "/uploads/" + fileName);
    }
    catch (...) {
        response.setStatus(500, "Internal Server Error");
        response.body = "<html><body><h1>File upload failed</h1></body></html>";
    }
    response.body = "<html><body><h1>Ciao, Mondo dal post!</h1></body></html>";
}

// Gestisce le richieste DELETE
void HTTPServer::handleDeleteRequest(const HTTPRequest &request, HTTPResponse &response) {
    // seleziona il server in base all'host della richiesta
    Server& server = getServerByHost(request, _config);

    // evita percorsi che contengono '..'
    if (request.uri.find("..") != std::string::npos) {
        response.setStatus(403, "Forbidden");
        response.body = "<html><body><h1>403 Forbidden</h1>"
                        "<p>Il percorso richiesto non &egrave; consentito.</p>"
                        "</body></html>";
        return;
    }

    const std::string prefix = "/uploads/";
    // controlla se l'URI inizia con /uploads/
    if (request.uri.compare(0, prefix.size(), prefix) != 0) {
        response.setStatus(403, "Forbidden");
        response.body = "<html><body><h1>403 Forbidden</h1>"
                        "<p>Accesso non autorizzato all'URI.</p>"
                        "</body></html>";
        return;
    }

    std::string fullpath = server.root[0] + request.uri;
    std::cout << "[DELETE] fullpath = " << fullpath << std::endl;

    struct stat fileStat;
    // verifica esistenza del file
    if (stat(fullpath.c_str(), &fileStat) != 0) {
        response.setStatus(404, "Not Found");
        try {
            std::string errorPage = server.getErrorPage("404");
            response.body = readFile(errorPage);
        } catch (...) {
            response.body = "<html><body><h1>404 Not Found</h1>"
                            "<p>Il file non esiste.</p>"
                            "</body></html>";
        }
        return;
    }
    // controllo: il path è un file regolare, non una directory
    if (!S_ISREG(fileStat.st_mode)) {
        response.setStatus(403, "Forbidden");
        response.body = "<html><body><h1>403 Forbidden</h1>"
                        "<p>Non &egrave; possibile eliminare una directory.</p>"
                        "</body></html>";
        return;
    }

    // tentativo di rimozione del file
    if (remove(fullpath.c_str()) != 0) {
        std::cerr << "[DELETE] remove() failed: " << std::strerror(errno) << std::endl;

        if (errno == ENOENT) {
            response.setStatus(404, "Not Found");
        } else {
            response.setStatus(500, "Internal Server Error");
        }
        try {
            std::string code;
            if (errno == ENOENT) {
                code = "404";
            } else {
                code = "500";
            }
            std::string errorPage = server.getErrorPage(code);
            response.body = readFile(errorPage);
        } catch (...) {
            response.body = "<html><body><h1>Errore</h1>"
                            "<p>Impossibile completare l'eliminazione del file.</p>"
                            "</body></html>";
        }
        return;
    }

    response.setStatus(200, "OK");
    response.body = "<html><body><h1>File eliminato con successo</h1></body></html>";
}

// Gestisce una singola richiesta del client
bool HTTPServer::handleClientRequest(ClientConnection *clientConn, const std::string &rawRequest) {
    HTTPRequest  request;
    HTTPResponse response;

    request.parseRequest(rawRequest);

    bool clientWantsKeepAlive = false;
    { // Gestione dell'intestazione Connection per persistenza
        std::string connHdr = getHeaderValue("Connection", request);
        std::transform(connHdr.begin(), connHdr.end(), connHdr.begin(), ::tolower);
        if (request.httpVersion == "HTTP/1.1")
            clientWantsKeepAlive = (connHdr != "close");
        else
            clientWantsKeepAlive = (connHdr == "keep-alive");
    }

    // Impostazione dello status in base al metodo della richiesta
    bool allowed = methodIsAllowed(request.uri, request.method, getServerByHost(request, _config));
    if (request.method != "GET" && request.method != "POST" && request.method != "DELETE")
        response.setStatus(400, "Bad Request");
    else
        response.setStatus(200, "OK");
    if (!allowed){
        response.setStatus(405, "Method not Allowed");
        try {
            std::string errorPage = (getServerByHost(request, _config)).getErrorPage("405");
            response.body =  readFile(errorPage);
        }
        catch (std::exception& e){
            response.body = "<html><body><h1>405 Forbidden</h1>"
                        "<p>default error page for 405 error.</p>"
                        "</body></html>";
        }
    }
    // Esecuzione CGI se richiesto tramite "cgi-bin" nell'URI
    else if (request.uri.find("cgi-bin") != std::string::npos) {
        CGIHandler cgi(request.uri);
        try {
            response.body = cgi.executeScript(request.method, request.body);
        } catch (std::exception &e) {
            response.setStatus(500, "Internal Server Error");
            response.body = "<html><body><h1>CGI Error</h1></body></html>";
        }
    }
    else if (request.method == "GET") {
        handleGetRequest(request, response);
    }
    else if (request.method == "POST") {
        handlePostRequest(request, response);
    }
    else {
        handleDeleteRequest(request, response);
    }

    // Impostazione degli header di risposta
    response.setHeader("Content-Type", "text/html");
    {
        std::ostringstream oss;
        oss << response.body.size();
        response.setHeader("Content-Length", oss.str());
    }
    response.setHeader("Connection", clientWantsKeepAlive ? "keep-alive" : "close");

    std::string respStr = response.toString();
    ssize_t sent = write(clientConn->getFd(), respStr.c_str(), respStr.size());
    if (sent < 0) {
        std::cerr << "Errore in write(): " << strerror(errno) << std::endl;
    }

    // Gestione keep-alive o chiusura connessione
    if (clientWantsKeepAlive) {
        size_t headerEnd = rawRequest.find("\r\n\r\n");
        size_t contentLength = 0;
        std::map<std::string, std::string>::const_iterator it
        = request.headers.find("Content-Length");
        if (it != request.headers.end())
            contentLength = std::atoi(it->second.c_str());
        size_t total = headerEnd + 4 + contentLength;
        clientConn->removeProcessedRequest(total);
        return true;
    } else {
        close(clientConn->getFd());
        return false;
    }
}

extern "C" void signalHandler(int signum) {
    std::cout << "\nReceived signal " << signum << ", shutting down server..." << std::endl;
    webserv_runo = 0;  // Usa la variabile globale che hai già definito
}


// Ciclo principale per la gestione degli eventi e delle connessioni
void HTTPServer::eventLoop() {

	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
    // Setup della struttura di polling per i socket
    std::vector<pollfd> pollfds;
    for (size_t s = 0; s < _listenSockets.size(); ++s) { // ciclo sui listen socket
        pollfd p;
        p.fd = _listenSockets[s];
        p.events = POLLIN;
        p.revents = 0;
        pollfds.push_back(p);
    }

    std::cout << "start event loop..." << std::endl;
    while (webserv_runo) { // ciclo principale di event loop
        int ret = poll(&pollfds[0], pollfds.size(), -1);
        if (ret < 0) {
            std::cerr << "Error in poll(): " << strerror(errno) << std::endl;
            continue;
        }

        for (size_t i = 0; i < pollfds.size(); ++i) { // ciclo sui file descriptor in pollfds
            if (!(pollfds[i].revents & POLLIN))
                continue;

            int fd = pollfds[i].fd;

            // Nuova connessione in ingresso se il fd è un listen socket
            if (std::find(_listenSockets.begin(), _listenSockets.end(), fd) 
                    != _listenSockets.end()) {
                sockaddr_in client_addr;
                socklen_t len = sizeof(client_addr);
                int client_fd = accept(fd, (struct sockaddr*)&client_addr, &len);
                if (client_fd < 0) {
                    std::cerr << "Error in accept(): " << strerror(errno) << std::endl;
                    continue;
                }
                int flags = fcntl(client_fd, F_GETFL, 0);
                fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

                std::cout << "new connection accepted: fd " << client_fd << std::endl;
                ClientConnection *newConn = new ClientConnection(client_fd);
                _clientConnections.push_back(newConn);
                pollfd cp = { client_fd, POLLIN, 0 };
                pollfds.push_back(cp);
                continue;
            }

            // Gestione dei dati in arrivo dalla connessione client
            ClientConnection *conn = NULL;
            for (size_t j = 0; j < _clientConnections.size(); ++j) { // ciclo per trovare la connessione corrispondente
                if (_clientConnections[j]->getFd() == fd) {
                    conn = _clientConnections[j];
                    break;
                }
            }
            if (!conn) continue;

            char buffer[4096];
            ssize_t n = read(fd, buffer, sizeof(buffer));
            if (n <= 0) { // chiude la connessione se non ci sono dati
                close(fd);
                pollfds.erase(pollfds.begin() + i);
                _clientConnections.erase(
                    std::remove(_clientConnections.begin(),
                                _clientConnections.end(),
                                conn),
                    _clientConnections.end());
                delete conn;
                --i;
                continue;
            }
            conn->getBuffer().append(buffer, n);

            // Processa tutte le richieste complete nel buffer del client
            while (conn->hasCompleteRequest()) { // ciclo per gestire richieste multiple
                std::string rawRequest = conn->getBuffer();
                bool keepAlive = handleClientRequest(conn, rawRequest);

                if (!keepAlive) {
                    close(fd);
                    pollfds.erase(pollfds.begin() + i);
                    _clientConnections.erase(
                        std::remove(_clientConnections.begin(),
                                    _clientConnections.end(),
                                    conn),
                        _clientConnections.end());
                    delete conn;
                    --i;
                    break;
                }
            }
        }
    }
	for (size_t i = 0; i < _clientConnections.size(); ++i)
        delete _clientConnections[i];
    _clientConnections.clear();
}

// Funzione per avviare il server (invoca il ciclo di evento)
void HTTPServer::run() {
    eventLoop();
}
