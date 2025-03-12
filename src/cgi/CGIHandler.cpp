#include "CGIHandler.hpp"

CGIHandler::CGIHandler(){}

CGIHandler::CGIHandler(std::string path): _cgipath(path) {
    std::cout << "--- constructo debug infos ---" << std::endl;
    std::cout << _cgipath << std::endl;
    std::cout << std::endl;
}

CGIHandler::~CGIHandler(){}

std::string CGIHandler::executeScript(const std::string& method, const std::string& query, const std::string& body) {
    int stdoutPipe[2], stdinPipe[2];
    if (pipe(stdoutPipe) == -1 || pipe(stdinPipe) == -1)
        throw CGIHandler::CGIerror("pipe creation error");

    pid_t pid = fork();
    if (pid == -1)
        throw CGIHandler::CGIerror("fork process error");

    else if (pid == 0) {
        dup2(stdinPipe[0], STDIN_FILENO);
        dup2(stdoutPipe[1], STDOUT_FILENO);

        // Chiudi i descrittori non necessari
        close(stdoutPipe[0]);
        close(stdoutPipe[1]);
        close(stdinPipe[0]);
        close(stdinPipe[1]);

        // Imposta variabili d'ambiente
        setenv("REQUEST_METHOD", method.c_str(), 1);
        setenv("QUERY_STRING", query.c_str(), 1);
        setenv("CONTENT_LENGTH", std::to_string(body.size()).c_str(), 1);

        execl(this->_cgipath.c_str(), this->_cgipath.c_str(), nullptr);
        exit(EXIT_FAILURE);
    } else { // Processo padre (test driver)
        // Scrivi il body nella pipe di input (se POST)
        write(stdinPipe[1], body.c_str(), body.size());
        close(stdinPipe[1]);

        // Leggi l'output dalla pipe di output
        close(stdoutPipe[1]);
        char buffer[4096];
        std::string output;
        while (read(stdoutPipe[0], buffer, sizeof(buffer)) > 0) {
            output += buffer;
        }
        close(stdoutPipe[0]);

        waitpid(pid, nullptr, 0); // Attendi la terminazione
        return output;
    }
}