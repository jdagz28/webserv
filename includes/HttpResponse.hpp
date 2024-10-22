/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 01:13:31 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/22 13:15:21 by jdagoy           ###   ########.fr       */
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

struct FileData
{
    std::string     filename;
    std::string     contentType;
    off_t           size;
    std::string     lastModified;

    bool operator<(const FileData& other) const
    {
        return (filename < other.filename);
    }
};


class HttpResponse
{
    private:
        HttpRequest                             &_request;
        Config                                  &_config;
        ServerConfig                            _serverConfig;
        LocationConfig                          _locationConfig;
        StatusCode                              _status;
        int                                     _client_socket; //!
        std::vector<std::string>                _allowedMethods; 
        std::vector<std::string>                _redirectDirective;
        std::string                             _redirect;
        std::string                             _serverName;

        std::map<std::string, std::string>      _headers;
        std::string                             _body;
        std::vector<unsigned char>              _responseMsg;
        
        
        HttpResponse(const HttpResponse &copy);
        HttpResponse &operator=(const HttpResponse &copy);
     
        void    processRequestGET();

        int checkMethod(const std::string &method);
        bool    isMethodAllowed(const LocationConfig &location, const std::string &requestMethod);
        bool    isKeepAlive() const;
        ServerConfig    checkLocConfigAndRequest();
        LocationConfig  getLocationConfig();
        std::string cleanURI(std::string uri);
        
        std::string comparePath(const ServerConfig &server, const HttpRequestLine &request);
        std::string resolvePath(const ServerConfig &server);
        std::string checkRoot(const ServerConfig &server, const std::string &path);
        std::string getDirectiveLoc(const std::string &directive);
        
        void getResourceContent(const std::string &file_path);
        bool isSupportedMedia(const std::string &uri);
        void getResource(const std::string &target_path);
        std::string extractResourceName(const std::string &uri);
        std::string buildResourcePath(const std::string &basePath, const std::string &resourceName);
        bool checkSlash(const std::string &defaultLoc, const std::string &page);
        std::string verifyPath(std::string path);

        std::string generateStatusLine();
        void addContentTypeHeader(const std::string &type);
        std::string getHttpDateGMT();
        void addKeepAliveHeader();
        void addAllowHeader();
        std::string generateHeaderLines();

        bool isRedirect(const LocationConfig &location);
        bool validateRedirect();
        bool isRedirectExternal();
        void getRedirectContent();
        void setRedirect(std::string status, const std::string &path);

        void generateDirList(std::string path);
        bool isAutoIndex();
        bool checkDirIndex();

        void getErrorPage();
        std::string generateErrorPage(const std::string &status, const std::string &statusMessage);

        
        void    processRequestPOST();
        // std::string generateFilename(const std::string &extension);
        void processImageUpload();
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