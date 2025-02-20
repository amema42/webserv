/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jacopo <jacopo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 18:42:32 by jacopo            #+#    #+#             */
/*   Updated: 2025/02/20 15:37:33 by jacopo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#if !defined(CGIHandler_hpp)
#define CGIHandler_hpp

#include "../../include/webServ.hpp"


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
			virtual const char* what() const throw()
			{
				return("error in CGI");
			}
    };
};

#endif // CGIHandler_hpp
