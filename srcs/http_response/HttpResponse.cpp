/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 01:19:13 by jdagoy            #+#    #+#             */
/*   Updated: 2024/08/30 11:06:29 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "webserv.hpp"
#include "ServerConfig.hpp"
#include "HttpRequestLine.hpp"
#include "LocationConfig.hpp"
#include <string>

HttpResponse::HttpResponse(HttpRequest &request,
                            Config &config,
                            int client_socket)
    : _request(request), _config(config), _status(0), _error(0), _errorMsg(""), _client_socket(client_socket)
{
}

HttpResponse::~HttpResponse()
{
}

int HttpResponse::checkMethod(const std::string &method)
{
    if (method == "GET")
        return (GET);
    else if (method == "POST")
        return (POST);
    else if (method == "DELETE")
        return (DELETE);
    return (ErrorMethod);
}

void HttpResponse::execMethod()
{
    std::string method = _request.getRequestLine().getMethod();
    

    switch (checkMethod(method))
    {
        case GET:
            processRequestGET();
            break ;
        default:
            _error = 1;
            _errorMsg = "Error: Method not implemented / unavailable.";
    }
}

bool HttpResponse::isMatchingPrefix(const std::string &pattern, const std::string &target)
{
    if (pattern.empty() || target.empty())
        return (false);
    std::string target_prefix = target.substr(0, pattern.length());
    return (pattern == target_prefix);
}

std::string HttpResponse::comparePath(const ServerConfig &server, const HttpRequestLine &request)
{
    std::string target_path = request.getUri();
    std::string path;
    const std::vector<LocationConfig> &locationConfigs = server.getLocationConfig();
    if (locationConfigs.empty())
        return(path);
    std::vector<LocationConfig>::const_iterator location;

    for (location = locationConfigs.begin(); location != server.getLocationConfig().end(); location++)
    {
        std::string config_location = location->getPath();

        if (config_location == target_path)
            return (config_location);
        if (target_path == "/")
        {
            if (path.empty() || config_location == "/")
                path = config_location;
            continue;
        }
        if (isMatchingPrefix(config_location, target_path))
            if (path.empty() || path.length() < config_location.length())
                path = config_location;
    }
    std::cout << "Path returned: " << path << std::endl;
    return (path);
}

bool HttpResponse::checkLocConfigAndRequest()
{
    const std::vector<ServerConfig> &serverConfigs = _config.getServerConfig();
    if (serverConfigs.empty())
        return (false);
    std::vector<ServerConfig>::const_iterator server;
    for (server = serverConfigs.begin(); server != serverConfigs.end(); server++)
    {
        std::string path = comparePath(*server, _request.getRequestLine());
        if (path.empty())
            return (false);
        if (!isMethodAllowed(*server, path, _request.getRequestLine()))
            return (false);
    }
    return (true);
}
bool HttpResponse::isMethodAllowed(const ServerConfig &server, const std::string &path, const HttpRequestLine &request)
{
    std::string requestMethod = request.getMethod();
    
    const std::vector<LocationConfig> &locationConfigs = server.getLocationConfig();
    if (locationConfigs.empty())
        return (false);
    std::vector<LocationConfig>::const_iterator location;

    for (location = locationConfigs.begin(); location != server.getLocationConfig().end(); location++)
    {
        std::string config_location = location->getPath();
        if (config_location == path)
        {
            if (location->isLimited())
            {
                if (location->isMethodExcluded(requestMethod))
                {
                    _error = 1;
                    _errorMsg = "Error: Method not allowed";
                    return (false);
                }
            }
            else
            {
                if (!location->isMethodAllowed(requestMethod))
                {
                    _error = 1;
                    _errorMsg = "Error: Method not allowed";
                    return (false);
                }
            }
            std::cout << requestMethod << " Method is Allowed" << std::endl;
            return (true);
        }
    }
    _error = 1;
    _errorMsg = "Error: Location not found or method not allowed";
    return (false);
    
}

std::string HttpResponse::checkRoot(const ServerConfig &server, const std::string &path)
{
    std::string rootpath;
    const std::vector<LocationConfig> &locationConfigs = server.getLocationConfig();
    if (locationConfigs.empty())
        return (rootpath);
    std::vector<LocationConfig>::const_iterator location;

    for (location = locationConfigs.begin(); location != server.getLocationConfig().end(); location++)
    {
        std::string config_location = location->getPath();
        if (config_location == path)
        {
            rootpath = location->getRoot();
            return (rootpath);
        }
    }
    return (rootpath);
}

std::string HttpResponse::resolvePath()
{
    const std::vector<ServerConfig> &serverConfigs = _config.getServerConfig();
    if (serverConfigs.empty())
    {
        _error = 1;
        _errorMsg = "Error: Server Config is empty";
        return (std::string());
    }
    std::vector<ServerConfig>::const_iterator server;
    for (server = serverConfigs.begin(); server != serverConfigs.end(); server++)
    {
        std::string path = comparePath(*server, _request.getRequestLine());
        if (path.empty())
            return (std::string());
        std::string root = checkRoot(*server, path);
        if (!root.empty())
            return (root + path);
        else
            return (path);
    }
    return (std::string());
}

std::string HttpResponse::getDefaultNameLoc(const ServerConfig &server)
{
    const std::vector<LocationConfig> &LocationConfigs = server.getLocationConfig();
    if (LocationConfigs.empty())
        return (std::string());
    std::vector<LocationConfig>::const_iterator location;
    for (location = LocationConfigs.begin(); location != LocationConfigs.end(); location++)
    {
        std::string path = comparePath(server, _request.getRequestLine());
        if (path.empty())
            return(std::string());
        if (location->getPath() == path)
            return (location->getDefaultName());
    }
    return (std::string());
}


std::string HttpResponse::getDefaultName()
{
    std::string defaultName;
    const std::vector<ServerConfig> &serverConfigs = _config.getServerConfig();
    if (serverConfigs.empty())
        return (std::string());
    std::vector<ServerConfig>::const_iterator server;
    for (server = serverConfigs.begin(); server != serverConfigs.end(); server++)
        defaultName = getDefaultNameLoc(*server);
    return (defaultName);
}

static bool checkSlash(const std::string &defaultLoc, const std::string &page)
{
    bool defaultSlash = !defaultLoc.empty() && defaultLoc[defaultLoc.length() - 1] == '/';
    bool pageSlash = !page.empty() && page[0] == '/';
    

    std::cout << defaultLoc << ":\t " << defaultSlash << std::endl;
    std::cout << page << ":\t" << pageSlash << std::endl;

    if (!defaultSlash && !pageSlash)
        return (false);
    return (true);
}

static std::string extractHTMLName(const std::string &uri)
{
    size_t  lastSlashPos = uri.find_last_of('/');
    if (lastSlashPos != std::string::npos && lastSlashPos + 1 < uri.length())
        return (uri.substr(lastSlashPos + 1));
    return (std::string());
}

void  HttpResponse::getContent(const std::string &file_path)
{
    std::ifstream   infile;
    std::string     buffer;
    
    infile.open(file_path.c_str());
    
    if (!infile.is_open())
    {
        _error = 404;
        _errorMsg = "Error: File not found";
        //error_page;
    }
    else
    {
        while (std::getline(infile, buffer)) 
            _body += buffer + "\n";
        _body += '\0'; 
        infile.close();
    }
    _contentLength = _body.size();
}

//! check if resource is supported (other than .html)
// constants of supported types
void HttpResponse::getIndexPage(const std::string &target_path)
{
    std::string indexPath;
    std::string defaultPage = getDefaultName();
    std::cout << "Default Page: " << defaultPage << std::endl;
    
    std::string uri = _request.getRequestLine().getUri();
    if (endsWith(uri, ".html"))
    {
        std::string pageName = extractHTMLName(uri);
        std::cout << "Page Name: " << pageName << std::endl;
        if (!checkSlash(target_path, pageName))
            indexPath = target_path + '/' + pageName;
        else
            indexPath = target_path + pageName;
        std::cout << "Index Path: " << indexPath << std::endl;
        if (fileExists(indexPath))
        {
            std::cout << "Page exists" << std::endl;
            //send file
            /** 
             * TODO: read html file and send
             * ! HttpResponse - repsonse atrributes
             * ? create a response class
            */ 
            getContent(indexPath);
            std::cout << "Response Body: " << _body << std::endl;
        }
        else
        {
            std::cout << "Page does not exist" << std::endl;
            //send 404
        }
    }
    else
    {
        if (!checkSlash(target_path, defaultPage))
            indexPath = target_path + '/' + defaultPage;
        else
            indexPath = target_path + defaultPage;
        getContent(indexPath);
        std::cout << "Response Body: " << _body << std::endl;
    }
}

//! Remove _error and _errorMsgs
//? Create a class Result to handle status code and message ??

void HttpResponse::processRequestGET()
{
    if (!checkLocConfigAndRequest())
    {
        if (!_error)
        {
            _error = 1;
            _errorMsg = "Error: Location not found";
        }
    }
    if (_error)
    {
        std::cout << _errorMsg << std::endl;
        return ;
    }

    /** 
     * TODO: check if redirect
     * * check if URI is absolute or relative; a redirection can be either
     */
    
    /** 
     * TODO: check if is directory
     * * check location config if auto index
     * * list
     */
    
    std::string path = resolvePath();
    if (path.empty())
    {
        _error = 1;
        _errorMsg = "Error: Path not found";
    }
    std::cout << "Resolved path: " << path << std::endl;
    
    if (isDirectory(path))
    {
        getIndexPage(path);
        
    }

}