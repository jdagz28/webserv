/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 01:19:13 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/11 21:43:28 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "webserv.hpp"
#include "ServerConfig.hpp"
#include "HttpRequestLine.hpp"
#include "LocationConfig.hpp"
#include <string>
#include <ctime>
#include <sstream>

HttpResponse::HttpResponse(HttpRequest &request,
                            Config &config,
                            int client_socket)
    : _request(request), _config(config), _status(INIT), _client_socket(client_socket), _allowedMethods(), _headers(), _body("")
{
}

HttpResponse::~HttpResponse()
{
}

void HttpResponse::setStatusCode(StatusCode status)
{
    _status = status;
}

StatusCode HttpResponse::getStatusCode() const
{
    return (_status);
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
            setStatusCode(METHOD_NOT_ALLOWED);
    }
}

static bool isMatchingPrefix(const std::string &pattern, const std::string &target)
{
    if (pattern.empty() || target.empty())
        return (false);
    std::string target_prefix = target.substr(0, pattern.length());
    if (pattern == target_prefix)
    {
        if (pattern == "/" && target != "/")
            return (false);
        return (true);
    }
    return (false);
}

std::string HttpResponse::comparePath(const ServerConfig &server, const HttpRequestLine &request)
{
    std::string target_path = request.getUri();
    std::string path;
    const std::vector<LocationConfig> &locationConfigs = server.getLocationConfig();
    if (locationConfigs.empty())
        return(std::string());
    std::vector<LocationConfig>::const_iterator location;

    for (location = locationConfigs.begin(); location != server.getLocationConfig().end(); location++)
    {
        std::string config_location = location->getPath();
        if (config_location == target_path || (target_path + "/") == config_location)
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
        _serverName = server->getServerName();
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
                if (!location->isLimitExcept(requestMethod))
                {
                    setStatusCode(METHOD_NOT_ALLOWED);
                    return (false);
                }
            }
            else
            {
                if (!location->isMethodAllowed(requestMethod))
                {
                    setStatusCode(METHOD_NOT_ALLOWED);
                    return (false);
                }
            }
            _allowedMethods = location->getAllowedMethods();
            return (true);
        }
    }
    setStatusCode(METHOD_NOT_ALLOWED);
    return (false);
    
}

std::string HttpResponse::checkRoot(const ServerConfig &server, const std::string &path)
{
    std::string rootpath;
    const std::vector<LocationConfig> &locationConfigs = server.getLocationConfig();
    if (locationConfigs.empty())
        return (std::string());
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
    return (std::string());
}

std::string HttpResponse::resolvePath()
{
    const std::vector<ServerConfig> &serverConfigs = _config.getServerConfig();
    if (serverConfigs.empty())
        return (std::string());
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

std::string HttpResponse::getDirectiveLoc(const ServerConfig &server, const std::string &directive)
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
        {
            if (directive == "default")
                return (location->getDefaultName());
            else if (directive == "index")
                return (location->getIndex());
            else if (directive == "autoindex")
                return (location->getAutoIndex());
        }
    }
    return (std::string());
}


std::string HttpResponse::getDirective(const std::string &directive)
{
    std::string defaultName;
    const std::vector<ServerConfig> &serverConfigs = _config.getServerConfig();
    if (serverConfigs.empty())
        return (std::string());
    std::vector<ServerConfig>::const_iterator server;
    for (server = serverConfigs.begin(); server != serverConfigs.end(); server++)
    {
        std::string path = comparePath(*server, _request.getRequestLine());
        if (path.empty())
            return (std::string());
        defaultName = getDirectiveLoc(*server, directive);
        if (!defaultName.empty())
            return (defaultName);
    }
    return (std::string());
}

bool HttpResponse::isSupportedMedia(const std::string &uri)
{
    std::string extension = getExtension(uri);
    std::string type = getMimeType(extension);
    return (!type.empty());    
}

void HttpResponse::sendResponse()
{
    if (_responseMsg.empty())
    {
        std::cerr << "ERROR: response empty" << std::endl;
        return;
    }

    ssize_t bytesSent = send(_client_socket, _responseMsg.data(), _responseMsg.size(), 0);
    if (bytesSent < 0)
    {
        std::cerr << "ERROR: sending bytes" << std::endl;
        return;
    }
    // std::cout << bytesSent << " bytes sent" << std::endl;
    _responseMsg.erase(_responseMsg.begin(), _responseMsg.begin() + bytesSent);
    if (_headers["Connection"] != "keep-alive")
    {
        if (close(_client_socket) < 0)
            std::cerr << "ERROR: closing socket" << std::endl;
    }
}

std::string HttpResponse::getHttpResponse()
{
    std::string response(_responseMsg.begin(), _responseMsg.end());
    return (response);
}

