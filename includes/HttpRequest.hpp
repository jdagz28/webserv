/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 02:11:42 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/04 01:24:37 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <vector>
#include <string>
#include "HttpRequestLine.hpp"


class HttpRequest
{
    private:
        HttpRequestLine                                                     _request;
        std::vector<std::pair<std::string, std::vector<std::string> > >     _headers;
        int                                                                 _headersN;
        int                                                                 _status;
        std::vector<unsigned char>                                          _buffer;
        int                                                                 _error;
        std::string                                                         _errorMsg;
        int                                                                 _client_socket;

        HttpRequest();
        HttpRequest(const HttpRequest &copy);
        HttpRequest &operator=(const HttpRequest &copy);

        void    parseHttpRequest();
        void    requestToBuffer(); 
        void    parseRequestLine(const std::string &line);
        void    parseRequestHeaders(const std::string &line);

        std::string     getLineAndPopFromBuffer();
        std::string     extract_token(const std::string &line, size_t &pos, char del);
        bool    isValidFieldName(const std::string &line);
        bool    isValidFieldValue(const std::string &line);
    
    public:
        HttpRequest(int client_socket);
        ~HttpRequest();

        void    setClientSocket(int client_socket);
        
        const HttpRequestLine& getRequestLine() const;
        const std::vector<std::pair<std::string, std::vector<std::string> > >& getHeaders() const;
        bool    isConnectionClosed() const;
        std::string getHost() const;

        void    printBuffer() const;

};


#endif
