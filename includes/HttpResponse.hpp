/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 01:13:31 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/04 23:58:54 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>
#include <vector>
#include "Config.hpp"
#include "webserv.hpp"

class HttpRequest;
class ServerConfig;
class HttpRequestLine;
class LocationConfig;


class HttpResponse
{
    private:
        HttpRequest                             &_request;
        Config                                  &_config;
        StatusCode                              _status;
        int                                     _client_socket; //!
        std::vector<std::string>                _allowedMethods; 
        std::vector<std::string>                _redirect;

        std::map<std::string, std::string>      _headers;
        std::string                             _body;
        std::vector<unsigned char>              _responseMsg;
        
        
        
        HttpResponse(const HttpResponse &copy);
        HttpResponse &operator=(const HttpResponse &copy);
     
        void    processRequestGET();

        int checkMethod(const std::string &method);
        bool    isMethodAllowed(const ServerConfig &server, const std::string &path, const HttpRequestLine &request);
        bool    isKeepAlive() const;
        bool    checkLocConfigAndRequest();
        
        std::string comparePath(const ServerConfig &server, const HttpRequestLine &request);
        std::string resolvePath();
        std::string checkRoot(const ServerConfig &server, const std::string &path);
        
        std::string getDefaultName();
        std::string getDefaultNameLoc(const ServerConfig &server);
        
        void getResourceContent(const std::string &file_path);
        bool isSupportedMedia(const std::string &uri);
        void getResource(const std::string &target_path);

        std::string generateStatusLine();
        void addContentTypeHeader(const std::string &type);
        std::string getHttpDateCET();
        void addKeepAliveHeader();
        void addAllowHeader();
        std::string generateHeaderLines();

    public:
        HttpResponse(HttpRequest &request,
                        Config &config,
                        int client_socket);
        ~HttpResponse();

        void    execMethod();
        void    setStatusCode(StatusCode status);
        void    generateHttpResponse();
        void    sendResponse();

        
        StatusCode getStatusCode() const;
    
        std::string getHttpResponse();
};


#endif