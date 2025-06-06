#if !defined(CGIHandler_hpp)
#define CGIHandler_hpp

#include "webServ.hpp"

class CGIHandler
{
    private:
    std::string _rawPath;
    std::string _cgipath;
    std::string _query; 

    public:
    CGIHandler();
    CGIHandler(std::string rawpath);
    ~CGIHandler();
    std::string executeScript(const std::string& method, const std::string& body);
    static volatile sig_atomic_t cgi_timeout_occurred;
    static const int CGI_TIMEOUT_SECONDS = 10;
    static void sigalrm_handler(int sig);

    
    
    class CGIerror: public std::exception {
        public:
            virtual ~CGIerror() throw(){};
            std::string _error;
            CGIerror(std::string error){
                _error = error;
            }
			virtual const char* what() const throw()
			{
				return(_error.c_str());
			}
    };
};

#endif // CGIHandler_hpp
