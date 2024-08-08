/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 01:19:13 by jdagoy            #+#    #+#             */
/*   Updated: 2024/08/08 04:52:50 by jdagoy           ###   ########.fr       */
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
            getRequestBody();
            break ;
        default:
            std::cout << "Error: Method not implemented / unavailable." << std::endl;
        
    }
}

bool HttpResponse::isMatchingPrefix(const std::string &pattern, const std::string &target)
{
    if (pattern.empty() || target.empty())
        return (false);
    std::cout << "CONFIG LOCATION: " << pattern << std::endl;
    std::cout << "TARGET PATH: " << target << std::endl << std::endl;
    std::string target_prefix = target.substr(0, pattern.length());
    return (pattern == target_prefix);
}

std::string HttpResponse::comparePath(const ServerConfig &server, const HttpRequestLine &request)
{
    std::string target_path = request.getUri();
    std::string path;
    const std::vector<LocationConfig> &locationConfigs = server.getLocationConfig();
    std::vector<LocationConfig>::const_iterator location;

    for (location = locationConfigs.begin(); location != server.getLocationConfig().end(); location++)
    {
        std::string config_location = location->getPath();
        //! check
        // std::cout << "CONFIG LOCATION: " << config_location << std::endl;
        // std::cout << "TARGET PATH: " << target_path << std::endl << std::endl;

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
    {
        return (false);
    }
    std::vector<ServerConfig>::const_iterator server;
    for (server = serverConfigs.begin(); server != serverConfigs.end(); server++)
    {
        std::string location = comparePath(*server, _request.getRequestLine());
        if (location.empty())
            return (false);
    }
    return (true);
}

bool HttpResponse::getLocationConfig()
{
    if (!checkLocConfigAndRequest())
        return (false);
    return (true);
}

bool HttpResponse::checkLimitExcept()
{
    if (!getLocationConfig())
        return (false);
    return (true);
}

bool HttpResponse::isMethodAllowed()
{
    if (!checkLimitExcept())
        return (false);
    return (true);
}

void HttpResponse::getRequestBody()
{
    // check method
    if (!isMethodAllowed())
    {
        _error = 1;
        _errorMsg = "Method not allowed.";
    }
    
    //check if redirect
}