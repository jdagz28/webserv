/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 01:13:31 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/19 02:51:49 by jdagoy           ###   ########.fr       */
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

struct MultiFormData
{
    std::string filename;
    std::string type;
    std::vector<unsigned char> binary;
};


class HttpResponse
{
    private:
        HttpRequest                             &_request;
        Config                                  &_config;
        StatusCode                              _status;
        int                                     _client_socket; //!
        std::vector<std::string>                _allowedMethods; 
        std::vector<std::string>                _redirectDirective;
        std::string                             _redirect;
        std::string                             _serverName;

        std::map<std::string, std::string>      _headers;
        std::string                             _body;
        std::vector<unsigned char>              _responseMsg;
        MultiFormData                           _formdata;
        
        
        
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
        
        std::string getDirective(const std::string &directive);
        std::string getDirectiveLoc(const ServerConfig &server, const std::string &directive);
        
        void getResourceContent(const std::string &file_path);
        bool isSupportedMedia(const std::string &uri);
        void getResource(const std::string &target_path);
        std::string extractResourceName(const std::string &uri);
        bool checkSlash(const std::string &defaultLoc, const std::string &page);

        std::string generateStatusLine();
        void addContentTypeHeader(const std::string &type);
        std::string getHttpDateGMT();
        void addKeepAliveHeader();
        void addAllowHeader();
        std::string generateHeaderLines();

        bool isRedirect();
        bool validateRedirect();
        bool isRedirectExternal();
        void getRedirectContent();
        void setRedirect(std::string status, const std::string &path);

        void generateDirList(const std::string &path);
        bool isAutoIndex();
        bool checkDirIndex();

        void getErrorPage();
        std::string generateErrorPage(const std::string &status, const std::string &statusMessage);

        
        void    processRequestPOST();
        bool    isMultiPartFormData(std::string *boundary);
        void    uploadMultipartForm(const std::string &boundary);
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