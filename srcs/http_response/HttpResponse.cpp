/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 01:19:13 by jdagoy            #+#    #+#             */
/*   Updated: 2025/01/14 22:28:04 by jdagoy          ###   ########.fr       */
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

HttpResponse::HttpResponse(HttpRequest &request, const Config &config, int client_socket)
    : _request(request), _config(config), _status(INIT), _client_socket(client_socket), _allowedMethods(), _headers(), _body("")
{
    if (_request.getStatusCode() !=  OK && _request.getStatusCode() != INIT)
    {
        _status = _request.getStatusCode();
        generateHttpResponse();
        return ;
    }
    execMethod();
    generateHttpResponse();
}

HttpResponse::~HttpResponse()
{
    _responseMsg.clear();
    _headers.clear();
    _allowedMethods.clear();
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
    _serverConfig = checkLocConfigAndRequest();
    if (!_serverConfig.isValid())
    {
        if (_status == INIT)
            setStatusCode(INTERNAL_SERVER_ERROR);
        return ;
    }

    _locationConfig = getLocationConfig();
    if (_locationConfig.getPath().empty())
    {
        setStatusCode(NOT_FOUND);
        return ;
    }
    
    if (!isMethodAllowed(_locationConfig, method))
        return ;
    
    switch (checkMethod(method))
    {
        case GET:
            processRequestGET();
            break ;
        case POST:
            _request.setMaxBodySize(_locationConfig.getClientMaxBodySize());
            _request.parseRequestBody();
            if (_request.getFormData("_method") == "DELETE")
            {
                processRequestDELETE();
                break ; 
            }
            processRequestPOST();
            break ;
        case DELETE:
            processRequestDELETE();
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
        if (target_path == "/index.html")
        {
            if (path.empty() || config_location == "/")
                path = config_location;
            continue;
        }

        if (isMatchingPrefix(config_location, target_path))
            if (path.empty() || path.length() < config_location.length())
                path = config_location;
    }
    if (path.empty())
        setStatusCode(NOT_FOUND);
    return (path);
}


LocationConfig HttpResponse::getLocationConfig()
{
    LocationConfig location;
    
    std::string path = comparePath(_serverConfig, _request.getRequestLine());
    if (path.empty())
        return (location);
    
    const std::vector<LocationConfig> &locationConfigs = _serverConfig.getLocationConfig();
    if (locationConfigs.empty())
        return (location);
    std::vector<LocationConfig>::const_iterator loc;
    for (loc = locationConfigs.begin(); loc != locationConfigs.end(); loc++)
    {
        if (loc->getPath() == path)
        {
            location = *loc;
            return (location);
        }
    }
    return (location);    
}

ServerConfig HttpResponse::checkLocConfigAndRequest()
{
    ServerConfig config;
    
    const std::vector<ServerConfig> &serverConfigs = _config.getServerConfig();
    if (serverConfigs.empty())
        return (config);
    std::string host = _request.getHost();
        if (host.empty())
            return (config);
    std::string requestHost;
    int port;
    size_t check = host.find(":");
    if (check != std::string::npos)
    {
        requestHost = host.substr(0, check);
        port = strToInt(host.substr(check + 1));
    }

    std::vector<ServerConfig>::const_iterator server;
    for (server = serverConfigs.begin(); server != serverConfigs.end(); server++)
    {
        std::string path = comparePath(*server, _request.getRequestLine());
        _serverName = server->checkServerName(requestHost);
        if (port == server->getPort())
        {
            config = *server;
            if (!path.empty())
                config.setValid();
            return (config);;
        }    
    }
    std::cout << "ERROR: Server not found" << std::endl;

    return (config);
}

bool HttpResponse::isMethodAllowed(const LocationConfig &location, const std::string &requestMethod)
{
    if (!location.isMethodAllowed(requestMethod))
    {
        setStatusCode(METHOD_NOT_ALLOWED);
        return (false);
    }
    _allowedMethods = location.getAllowedMethods();
    return (true);
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

std::string HttpResponse::resolvePath(const ServerConfig &server)
{
    std::string path = comparePath(server, _request.getRequestLine());
    if (path.empty())
        return (std::string());
    std::string root = checkRoot(server, path);
    if (!root.empty())
        return (root + path);
    else
        return (path);
}

std::string HttpResponse::getDirectiveLoc(const std::string &directive)
{

    if (directive == "index")
        return (_locationConfig.getIndex());
    else if (directive == "autoindex")
        return (_locationConfig.getAutoIndex());
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
        std::cerr << "ERROR: sending bytes" << std::endl; //!Change
        return;
    }
    _responseMsg.erase(_responseMsg.begin(), _responseMsg.begin() + bytesSent);
    if (_headers["Connection"] != "keep-alive")
    {
        if (close(_client_socket) < 0)
            std::cerr << "ERROR: closing socket" << std::endl; //! CHECK
    }
}

std::string HttpResponse::getHttpResponse()
{
    std::string response(_responseMsg.begin(), _responseMsg.end());
    return (response);
}

std::string HttpResponse::cleanURI(std::string uri)
{
    while (uri.find("//") != std::string::npos)
        uri.erase(uri.find("//"), 1);
    if (!uri.empty() && uri[uri.size() - 1] == '/')
        uri = uri.substr(0, uri.size() - 1);
    if (uri[0] == '/')
        uri = uri.substr(1);
    return (uri);
}