/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Redirect.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 01:02:29 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/19 03:04:48 by jdagoy           ###   ########.fr       */
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

bool HttpResponse::isRedirect()
{
    const std::vector<ServerConfig> &serverConfigs = _config.getServerConfig();
    if (serverConfigs.empty())
        return (false);
    std::vector<ServerConfig>::const_iterator server;
    for (server = serverConfigs.begin(); server != serverConfigs.end(); server++)
    {
        std::string path = comparePath(*server, _request.getRequestLine());
        if (path.empty())
            continue ;
        const std::vector<LocationConfig> &locationConfigs = server->getLocationConfig();
        if (locationConfigs.empty())
            return (false);
        std::vector<LocationConfig>::const_iterator location;
        for (location = locationConfigs.begin(); location != locationConfigs.end(); location++)
        {
            std::string path = comparePath(*server, _request.getRequestLine());
            if (path.empty())
                continue ;
            if (location->getPath() == path)
            {
                if (location->isRedirect())
                {
                    _redirectDirective = location->getRedirect();
                    return (true);
                }
            }
        }
    }
    return (false);
}

bool HttpResponse::validateRedirect()
{
    if (_redirectDirective.size() != 2)
        return (false);
    int redirectStatus = strToInt(_redirectDirective[0]);
    if (redirectStatus < 300 || redirectStatus > 308)
        return (false);
    _redirect = _redirectDirective[1];
    return (true);
}

bool HttpResponse::isRedirectExternal()
{
    if (_redirect.empty())
        return (false);
    else if (!validProtocol(_redirect))
        return (false);
    else
    {
        std::string http = "http://";
        std::string https = "https://";
        std::string localhost = "localhost:";
        
        if (_redirect.substr(0, http.length()) == http)
        {
            if (_redirect.substr(http.length(), localhost.length()) == localhost)
                return (false);
        }
        else if (_redirect.substr(0, https.length()) == https)
        {
            if (_redirect.substr(https.length(), localhost.length()) == localhost)
                return (false);
        }
    }
    return (true);
}

void HttpResponse::getRedirectContent()
{
    std::string redirectPath;
    if (!validateRedirect())
       return ;
    if (_request.getHeader("content-type") == "application/x-www-form-urlencoded")
    {
        redirectPath = _redirect;
        redirectPath += '?';
        std::map<std::string, std::string>::const_iterator value;
        for (value = _request.getFormData().begin(); value != _request.getFormData().end(); value++)
        {
            redirectPath += value->first + "=" + value->second + "&";
        }
    }
    else if (isRedirectExternal())
        redirectPath = _redirect; 
    else
    {
        redirectPath = "http://" + _request.getHost();
        if (!_redirect.empty() && _redirect[0] != '/')
            redirectPath += '/';
        redirectPath += _redirect;
    }
    _headers["Location"] = redirectPath;
    StatusCode status = static_cast<StatusCode>(strToInt(_redirectDirective[0]));
    setStatusCode(status);
}

void HttpResponse::setRedirect(std::string status, const std::string &path)
{
    _redirectDirective.push_back(status);
    _redirectDirective.push_back(path);
}