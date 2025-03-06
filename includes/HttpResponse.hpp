/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 01:13:31 by jdagoy            #+#    #+#             */
/*   Updated: 2025/03/06 04:02:12 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>
#include <vector>
#include <set>
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
        const Config                            &_config;
        ServerConfig                            _serverConfig;
        LocationConfig                          _locationConfig;
        StatusCode                              _status;
        int                                     _client_socket; 
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
        std::string checkRoot(const std::string &path);
        std::string getDirectiveLoc(const std::string &directive);
        
        bool    isSupportedMedia(const std::string &uri);
        bool    checkSlash(const std::string &defaultLoc, const std::string &page);
        void    getResourceContent(const std::string &file_path);
        void    getResource(const std::string &target_path);
        std::string extractResourceName(const std::string &uri);
        std::string buildResourcePath(const std::string &basePath, const std::string &resourceName);
        std::string verifyPath(std::string path);

        std::string generateStatusLine();
        std::string generateHeaderLines();
        std::string getHttpDateGMT();
        void    addContentTypeHeader(const std::string &type);
        void    addKeepAliveHeader();
        void    addAllowHeader();

        bool    isRedirect(const LocationConfig &location);
        bool    validateRedirect();
        bool    isRedirectExternal();
        void    getRedirectContent();
        void    setRedirect(std::string status, const std::string &path);

        void    generateDirList(std::string path);
        void    generateDirPage(const std::string &path, std::set<FileData> &directories, std::set<FileData> &files);
        bool    isAutoIndex();
        bool    checkDirIndex();
        

        void    getErrorPage();
        void	generateErrorPage(const std::string &status, const std::string &statusMessage);

        void    processRequestPOST();
        void    processImageUpload();
        bool	checkPostLocation();
		void	processForm();
		std::string	generatePostMessage(const std::string &message);

        void    processRequestDELETE();
        void    deleteFile(const std::string &file);
        void    curlDelete();
        
    public:
        HttpResponse(HttpRequest &request, const Config &config, int client_socket);
        ~HttpResponse();

        void    execMethod();
        void    setStatusCode(StatusCode status);
        void    generateHttpResponse();
        void    sendResponse();

        
        StatusCode getStatusCode() const;
        std::string	getHttpResponse() const;
        std::string	getHeader(const std::string &header);
        std::string	getStatusLine();
};


#endif