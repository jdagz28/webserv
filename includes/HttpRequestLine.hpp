/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestLine.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 03:32:51 by jdagoy            #+#    #+#             */
/*   Updated: 2025/02/11 10:12:46 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUESTLINE_HPP
#define HTTPREQUESTLINE_HPP

#include <string>


class   HttpRequestLine
{
    private:
        std::string     _method;
        std::string     _uri;
        std::string     _version;    
        int             _error;
        std::string     _errormsg;
        
    public:
        HttpRequestLine();
        HttpRequestLine(const HttpRequestLine &copy);
        HttpRequestLine &operator=(const HttpRequestLine &copy);
        ~HttpRequestLine();

        void    setMethod(const std::string &method);
        void    setUri(const std::string &uri);
        void    setVersion(const std::string &version);

        const std::string	&getMethod() const;
        const std::string	&getUri() const;
        const std::string	&getVersion() const;
        const std::string	&getErrormsg() const;
        int   getError() const;
};


#endif