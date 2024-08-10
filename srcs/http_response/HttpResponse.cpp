/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 01:19:13 by jdagoy            #+#    #+#             */
/*   Updated: 2024/08/10 06:57:21 by jdagoy           ###   ########.fr       */
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
            // std::cout << "Method: " << method << std::endl;
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
    // std::cout << "CONFIG LOCATION: " << pattern << std::endl;
    // std::cout << "TARGET PATH: " << target << std::endl << std::endl;
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
        {
            std::cout << "Exact match found: " << config_location << std::endl;
            return (config_location);
        }
        if (target_path == "/")
        {
            if (path.empty() || config_location == "/")
                path = config_location;
            std::cout << "Using config loc: " << config_location << std::endl;
            continue;
        }
        if (isMatchingPrefix(config_location, target_path))
        {
            if (path.empty() || path.length() < config_location.length())
            {
                path = config_location;
                std::cout << "Best match updated to: " << path << std::endl;
            }
        }
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

void HttpResponse::processRequestGET()
{
    // check location and method
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
    
    //check if redirect; handle if yes


    // check if path is directory or file
    std::string path = resolvePath();
    if (path.empty())
    {
        _error = 1;
        _errorMsg = "Error: Path not found";
    }
    std::cout << "Resolved path: " << path << std::endl;
    
    // if (!isDirectory())
    // {
    //     _error = 1;
    //     _errorMsg = "Error: Path is not a directory";
    // }

}