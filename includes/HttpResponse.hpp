/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 01:13:31 by jdagoy            #+#    #+#             */
/*   Updated: 2024/08/10 04:59:09 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "Config.hpp"

class HttpRequest;
class ServerConfig;
class HttpRequestLine;


class HttpResponse
{
    private:
        HttpRequest     &_request;
        Config          &_config;
        int             _status;
        int             _error;
        std::string     _errorMsg;
        int             _client_socket;
        
        
        HttpResponse(const HttpResponse &copy);
        HttpResponse &operator=(const HttpResponse &copy);

        int checkMethod(const std::string &method);
        void    getRequestBody();
        bool    isMethodAllowed(const ServerConfig &server, const std::string &path, const HttpRequestLine &request);
        bool    isMatchingPrefix(const std::string &pattern, const std::string &target);
        std::string    comparePath(const ServerConfig &server, const HttpRequestLine &request);
        bool    checkLocConfigAndRequest();


    public:
        HttpResponse(HttpRequest &request,
                        Config &config,
                        int client_socket);
        ~HttpResponse();

        void    execMethod();
    
        
};


#endif