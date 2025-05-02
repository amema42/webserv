#include "CGIHandler.hpp"

CGIHandler::CGIHandler(){
	_cgipath = "basic init";
}

CGIHandler::CGIHandler(std::string rawpath): _rawPath(rawpath) {
    std::cout << "--- constructo debug infos ---" << std::endl;
    std::size_t pos = _rawPath.find("?");
    _cgipath = _rawPath.substr(1, pos - 1);
    _query = _rawPath.substr(pos + 1);
    std::cout << "the cgi path" << _cgipath << std::endl;
    std::cout << "query string" << _query << std::endl;
    std::cout << std::endl;
}

CGIHandler::~CGIHandler(){}

std::string CGIHandler::executeScript(const std::string& method, const std::string& body) {
    int stdoutPipe[2], stdinPipe[2];
    std::ifstream file(_cgipath.c_str());
    if (!file.is_open())
        throw CGIHandler::CGIerror("unable to find a cgi script");

    if (pipe(stdoutPipe) == -1 || pipe(stdinPipe) == -1)
        throw CGIHandler::CGIerror("pipe creation error");

    pid_t pid = fork();
    if (pid == -1)
        throw CGIHandler::CGIerror("fork process error");

    else if (pid == 0) {
        std::stringstream number;
        number << body.size();
        
        dup2(stdinPipe[0], STDIN_FILENO);
        dup2(stdoutPipe[1], STDOUT_FILENO);

        // Chiudi i descrittori non necessari
        close(stdoutPipe[0]);
        close(stdoutPipe[1]);
        close(stdinPipe[0]);
        close(stdinPipe[1]);

        // Imposta variabili d'ambiente
        setenv("REQUEST_METHOD", method.c_str(), 1);
        setenv("QUERY_STRING", _query.c_str(), 1);
        setenv("CONTENT_LENGTH", (number.str()).c_str(), 1);

        execl(this->_cgipath.c_str(), this->_cgipath.c_str(), NULL);
        exit(EXIT_FAILURE);
    } else { // Processo padre (test driver)
        // Scrivi il body nella pipe di input (se POST)
        write(stdinPipe[1], body.c_str(), body.size());
        close(stdinPipe[1]);

        // Leggi l'output dalla pipe di output
        // Leggi l'output dalla pipe di output
		close(stdoutPipe[1]);
		char buffer[4096];
		std::string output;
		int bytesRead;
		while ((bytesRead = read(stdoutPipe[0], buffer, sizeof(buffer))) > 0) {
		    output.append(buffer, bytesRead); // Append only the read bytes
		}
		close(stdoutPipe[0]);

        waitpid(pid, NULL, 0); // Attendi la terminazione
        return output;
    }
}