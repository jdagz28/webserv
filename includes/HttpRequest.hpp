/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 02:11:42 by jdagoy            #+#    #+#             */
/*   Updated: 2024/08/01 04:26:12 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPRREQUEST_HPP

#include <map>
#include <vector>
#include <string>
#include "HttpRequestLine.hpp"

class HttpRequest
{
    private:
        HttpRequestLine                         _request;
        std::map<std::string, std::string>      _headers;
        std::string                             _body;
        int                                     _status;
        std::vector<unsigned char>              _buffer;
        int                                     _error;
        int                                     _client_socket;

        HttpRequest(const HttpRequest &copy);
        HttpRequest &operator=(const HttpRequest &copy);

        void    parseHttpRequest();
        void    requestToBuffer(); 
        void    parseRequestLine(const std::string &line);
        // void    parseRequestHeader();
        // void    parseRequestBody();

        std::string     getLineAndPopFromBuffer();
        std::string     get_line(const std::vector<unsigned char> &buffer,
                                    std::vector<unsigned char>::const_iterator start,
                                    std::vector<unsigned char>::const_iterator* end);
        std::string     extract_token(const std::string &line, size_t &pos, char del);
    
    public:
        HttpRequest(int client_socket);
        ~HttpRequest();

        void    setClientSocket(int client_socket);

        void    printBuffer() const;

};


#endif
