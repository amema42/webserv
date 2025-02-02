// #include "HTTPRequest.hpp"
// #include <iostream>
// #include <string>

// int main() {
//     // Esempio di richiesta HTTP semplice (no body)
//     std::string rawRequest =
//         // -- test GET -- Status: Ok
//         // "GET /index.html HTTP/1.1\r\n"
//         // "Host: www.example.com\r\n"
//         // "User-Agent: TestAgent/1.0\r\n"
//         // "Accept: text/html\r\n"
//         // "\r\n";

//         // -- test POST -- Status: Ok
//         // "POST /submit-form HTTP/1.1\r\n"
//         // "Host: www.example.com\r\n"
//         // "Content-Type: application/x-www-form-urlencoded\r\n"
//         // "Content-Length: 27\r\n"
//         // "\r\n"
//         // "field1=value1&field2=value2";
        
//         // -- test2 POST chunked -- Status: - ToDo: Decodifica del Chunked Transfer-Encoding:
//         "POST /upload HTTP/1.1\r\n"
//         "Host: www.example.com\r\n"
//         "Transfer-Encoding: chunked\r\n" // ToDo: Decodifica del Chunked Transfer-Encoding:
//         "\r\n"
//         "5\r\nHello\r\n0\r\n\r\n";

//     HTTPRequest request;
//     request.parseRequest(rawRequest);

//     // Stampa dei risultati
//     std::cout << "Metodo: " << request.method << std::endl;
//     std::cout << "URI: " << request.uri << std::endl;
//     std::cout << "HTTP Version: " << request.httpVersion << std::endl;

//     std::cout << "Headers:" << std::endl;
//     for (std::map<std::string, std::string>::const_iterator it = request.headers.begin();
//          it != request.headers.end(); ++it)
//     {
//         std::cout << "\t" << it->first << ": " << it->second << std::endl;
//     }

//     std::cout << "Body: " << request.body << std::endl;

//     return 0;
// }