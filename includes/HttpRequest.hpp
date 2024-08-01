/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 02:11:42 by jdagoy            #+#    #+#             */
/*   Updated: 2024/08/01 01:48:42 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPRREQUEST_HPP

#include <map>
#include <vector>
#include <string>

class HttpRequest
{
    private:
        std::string                             _method;
        std::string                             _uri;
        std::string                             _version;
        std::map<std::string, std::string>      _headers;
        std::string                             _body;
        int                                     _status;
        std::vector<unsigned char>              _buffer;
        int                                     _error;

        void    parseHttpRequest();
        // void    parseRequestLine();
        // void    parseRequestHeader();
        // void    parseRequestBody();

        std::string     getLineAndPopFromBuffer();
        std::string     get_line(const std::vector<unsigned char> &buffer,
                                    std::vector<unsigned char>::const_iterator start,
                                    std::vector<unsigned char>::const_iterator* end);
    
    public:
        HttpRequest();
        HttpRequest(const HttpRequest &copy);
        HttpRequest &operator=(const HttpRequest &copy);
        ~HttpRequest();

        void    requestToBuffer(int client_socket); 

        void    printBuffer() const;

};


#endif
