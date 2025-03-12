#if !defined(CGIHandler_hpp)
#define CGIHandler_hpp

#include "webServ.hpp"

class CGIHandler
{
    private:
    std::string _cgipath;

    public:
    CGIHandler();
    CGIHandler(std::string path);
    ~CGIHandler();
    std::string executeScript(const std::string& method, const std::string& query, const std::string& body);
    

    
    
    class CGIerror: public std::exception {
        public:
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
