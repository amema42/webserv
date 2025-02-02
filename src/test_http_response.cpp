#include "HTTPResponse.hpp"
#include <iostream>
#include <sstream>

// Simple response tester
int main() {
    HTTPResponse response;

    // setting some personalized test headers
    response.setHeader("Content-Type", "text/plain");
    response.setHeader("Connection", "close");

    // test body response for C++98
    response.body = "Tim Berners-Lee, il padre del World Wide Web, ha inventato il web nel 1989 || Paolo Brosio, il padre del Web, ha inventato il web nel 1989";
    // If we don't set "Content-Length", we can do it automatically 
    // {
    std::ostringstream oss;
    oss << response.body.size();
    response.setHeader("Content-Length", oss.str());
    // }
    //response.setHeader("Content-Length", std::to_string(response.body.size())); /*Can't use this cause std::to_string doesn't exist on C++98 */


    // Printf final response
    // Printf final response
    std::cout << "Risposta HTTP generata:\n";
    std::cout << response.toString() << std::endl;

    return 0;
}
