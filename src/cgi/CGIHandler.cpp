#include "CGIHandler.hpp"

CGIHandler::CGIHandler(){
	_cgipath = "basic init";
}

volatile sig_atomic_t CGIHandler::cgi_timeout_occurred = 0;

CGIHandler::CGIHandler(std::string rawpath): _rawPath(rawpath) {
    std::cout << "--- constructo debug infos ---" << std::endl;
    std::size_t pos = _rawPath.find("?");
    _cgipath = _rawPath.substr(1, pos - 1);
    _query = _rawPath.substr(pos + 1);
    std::cout << "the cgi path" << _cgipath << std::endl;
    std::cout << "query string" << _query << std::endl;
    std::cout << std::endl;
}

void CGIHandler::sigalrm_handler(int sig) {
    (void)sig; // Per evitare warning di parametro non usato
    cgi_timeout_occurred = 1;
}

CGIHandler::~CGIHandler(){}

std::string CGIHandler::executeScript(const std::string& method, const std::string& body) {
    int stdoutPipe[2], stdinPipe[2];

    std::ifstream file_check(_cgipath.c_str());
    if (!file_check.is_open()) {
        throw CGIHandler::CGIerror("Unable to find CGI script: " + _cgipath);
    }
    file_check.close();

    if (pipe(stdoutPipe) == -1 || pipe(stdinPipe) == -1) {
        throw CGIHandler::CGIerror("Pipe creation error");
    }

    pid_t pid = fork();
    if (pid == -1) {
        close(stdoutPipe[0]); close(stdoutPipe[1]);
        close(stdinPipe[0]);  close(stdinPipe[1]);
        throw CGIHandler::CGIerror("Fork process error");
    }

    if (pid == 0) {
        if (dup2(stdinPipe[0], STDIN_FILENO) == -1) { perror("dup2 stdinPipe"); exit(EXIT_FAILURE); }
        if (dup2(stdoutPipe[1], STDOUT_FILENO) == -1) { perror("dup2 stdoutPipe"); exit(EXIT_FAILURE); }

        close(stdinPipe[0]);  close(stdinPipe[1]);
        close(stdoutPipe[0]); close(stdoutPipe[1]);

        std::stringstream contentLengthStr;
        contentLengthStr << body.size();
        
        setenv("REQUEST_METHOD", method.c_str(), 1);
        setenv("QUERY_STRING", _query.c_str(), 1);
        setenv("CONTENT_LENGTH", contentLengthStr.str().c_str(), 1);
        setenv("SCRIPT_FILENAME", _cgipath.c_str(), 1);

       // execl(_cgipath.c_str(), _cgipath.c_str(), (char *)NULL);
       execlp("python3", "python3", _cgipath.c_str(), (char *)NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);

    } else {
        close(stdinPipe[0]);
        close(stdoutPipe[1]);

        struct sigaction sa;
        sa.sa_handler = sigalrm_handler; // Imposta il nostro gestore
        sigemptyset(&sa.sa_mask);        // Non bloccare altri segnali durante l'esecuzione del gestore
        sa.sa_flags = 0;                 // Importante: NON impostare SA_RESTART.
                                         // Questo assicura che le chiamate di sistema (es. read, write)
                                         // vengano interrotte dal segnale e restituiscano EINTR.
        if (sigaction(SIGALRM, &sa, NULL) == -1) {
            // Errore nell'impostare il gestore, tentativo di pulizia
            perror("sigaction failed");
            close(stdoutPipe[0]); close(stdinPipe[1]); // Chiudi le pipe rimanenti
            kill(pid, SIGKILL);    // Tenta di terminare il processo figlio
            waitpid(pid, NULL, 0); // Raccogli il figlio per evitare zombie
            throw CGIHandler::CGIerror("Failed to set SIGALRM handler");
        }
        
        cgi_timeout_occurred = 0;
        alarm(CGI_TIMEOUT_SECONDS);  

        if (!body.empty()) {
            ssize_t bytes_written = write(stdinPipe[1], body.c_str(), body.size());
            if (bytes_written == -1) {
                alarm(0);
                close(stdoutPipe[0]);
                close(stdinPipe[1]);
                kill(pid, SIGKILL); waitpid(pid, NULL, 0);
                throw CGIHandler::CGIerror("Error writing to CGI script stdin");
            }
        }
        close(stdinPipe[1]);

        std::string output;
        char buffer[4096];
        ssize_t bytesRead;

        while (true) {
            bytesRead = read(stdoutPipe[0], buffer, sizeof(buffer));
            if (bytesRead > 0) {
                output.append(buffer, bytesRead);
            } else if (bytesRead == 0) {
                break; //teah we read correctly every bite :D
            } else {
                    if (cgi_timeout_occurred) {
                        break;
                    }
                else {
                    alarm(0);
                    close(stdoutPipe[0]);
                    kill(pid, SIGKILL);
                    waitpid(pid, NULL, 0);
                    throw CGIHandler::CGIerror("Read error from CGI script stdout");
                }
            }
        }
        alarm(0);
        close(stdoutPipe[0]);

        int status;
        if (cgi_timeout_occurred) {
            kill(pid, SIGKILL);
            waitpid(pid, NULL, 0); // Raccogli il processo figlio (evita zombie)
            throw CGIHandler::CGIerror("CGI script execution timed out");
        } else {
            if (waitpid(pid, &status, 0) == -1) {
                throw CGIHandler::CGIerror("waitpid error after CGI script execution");
            }
        }
        return output;
    }
}