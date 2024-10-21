/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 02:19:46 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/21 05:17:05 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"
#include <cstdlib>

ServerConfig::ServerConfig()
{
}

ServerConfig::ServerConfig(const ServerConfig &copy)
{
    _directives = copy._directives;
    _port = copy._port;
    _serverName = copy._serverName;
    _locationConfig = copy._locationConfig;
    _errorPages = copy._errorPages;
    _locationPaths = copy._locationPaths;
}

ServerConfig::~ServerConfig()
{}

ServerConfig    &ServerConfig::operator=(const ServerConfig &copy)
{
    if (this != &copy)
    {
        _directives = copy._directives;
        _port = copy._port;
        _serverName = copy._serverName;
        _locationConfig = copy._locationConfig;
        _errorPages = copy._errorPages;
        _locationPaths = copy._locationPaths;
    }
    return (*this);
}

void   ServerConfig::setDirective(const std::string &directive, const std::string &value)
{
    _directives[directive].push_back(value);
}

void    ServerConfig::setPort(const std::string &port)
{
    _port = strToInt(port);
}

void    ServerConfig::setServerName(const std::string &name)
{
    std::vector<std::string>::iterator it;
    for (it = _serverName.begin(); it != _serverName.end(); it++)
    {
        if (*it == name)
            return ;
    }
    _serverName.push_back(name);
}

void    ServerConfig::setLocationConfig(const LocationConfig &locationConfig)
{
    _locationConfig.push_back(locationConfig);
}

void ServerConfig::setErrorPage(int errorCode, const std::string &errorPagePath)
{
    _errorPages[errorCode] = errorPagePath;
}

const std::map<std::string, std::vector<std::string> > &ServerConfig::getDirectives() const
{
    return (_directives);
}

int ServerConfig::getPort() const
{
    return (_port);
}

std::string ServerConfig::getServerName() const
{
    if (_serverName.empty())
        return (std::string());
    std::vector<std::string>::const_iterator it;
    std::string names;
    for (it = _serverName.begin(); it != _serverName.end(); it++)
    {
        names += *it;
        names += " ";
    }
    return (names);
}

std::string ServerConfig::checkServerName(const std::string &requestHost) const
{
    if (_serverName.empty())
        return (DEFAULT_SERVERNAME);
    std::vector<std::string>::const_iterator it;
    for (it = _serverName.begin(); it != _serverName.end(); it++)
    {
        if (*it == requestHost)
            return (requestHost);
    }
    return (DEFAULT_SERVERNAME);
}

const std::vector<LocationConfig> &ServerConfig::getLocationConfig() const
{
    return (_locationConfig);
}

const std::string ServerConfig::getErrorPage(StatusCode status) const
{    
    std::map<int, std::string>::const_iterator it = _errorPages.find(status);
    if (it == _errorPages.end())
        return (std::string());
    return (it->second);
}

void ServerConfig::setLocationPath(const std::string &path)
{
    _locationPaths.push_back(path);
}

bool ServerConfig::isPathAlreadySet(const std::string &path) const
{
    std::vector<std::string>::const_iterator it;
    for (it = _locationPaths.begin(); it != _locationPaths.end(); it++)
    {
        if (*it == path)
            return (true);
    }
    return (false);
}