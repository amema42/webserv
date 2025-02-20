#include "CGIHandler.hpp"

int main() {
    CGIHandler cgi("../../www/cgi-bin/test.py");
    {

        std::string method = "GET";
        std::string query = "name=John&age=30";
        std::string body = "";
        
        std::string output = cgi.executeScript(method, query, body);
        std::cout << "*** Risposta CGI ***\n" << output << std::endl;
    }
    std::cout << "questo è il post" << std::endl;
    {

        std::string method = "POST";
        std::string query = "name=John&age=30";
        std::string body = "cscsdcdscdscdcscdcdcd";
        
        std::string output = cgi.executeScript(method, query, body);
        std::cout << "*** Risposta CGI ***\n" << output << std::endl;
        return 0;
    }
}
