/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 02:19:46 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/07 13:26:55 by jdagoy           ###   ########.fr       */
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
    _address = copy._address;
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
        _address = copy._address;
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

void    ServerConfig::setPort(const std::string &address)
{
    _address.push_back(address);
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

std::string ServerConfig::getPort() const
{
    if (_address.empty())
        return (std::string());
    std::vector<std::string>::const_iterator it;
    std::string ports;
    for (it = _address.begin(); it != _address.end(); it++)
    {
        size_t pos = it->find(':');
        if (pos != std::string::npos)
        {    
            ports += it->substr(pos + 1);
            ports += " ";
        }
    }
    return (ports);
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