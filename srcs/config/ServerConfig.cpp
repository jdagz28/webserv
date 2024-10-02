/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 02:19:46 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/02 15:29:22 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"
#include <cstdlib>

ServerConfig::ServerConfig()
    : _port(-1), _serverName("")
{
}

ServerConfig::ServerConfig(const ServerConfig &copy)
{
    _port = copy.getPort();
    _serverName = copy.getServerName();
    _locationConfig = copy.getLocationConfig();
    _directives = copy.getDirectives();
}

ServerConfig::~ServerConfig()
{}

ServerConfig    &ServerConfig::operator=(const ServerConfig &copy)
{
    _port = copy.getPort();
    _serverName = copy.getServerName();
    _locationConfig = copy.getLocationConfig();
    _directives = copy.getDirectives();
    return (*this);
}

void   ServerConfig::setDirective(const std::string &directive, const std::string &value)
{
    _directives[directive].push_back(value);
}

void    ServerConfig::setPort(const std::string &port)
{
    std::string portStr = port;
    
    size_t colonPos = port.find(':');
    if (colonPos != std::string::npos)
    {
        portStr = port.substr(colonPos + 1);
        _port = atoi(portStr.c_str());
    }
    else
        _port = atoi(port.c_str());
}

void    ServerConfig::setServerName(const std::string &name)
{
    _serverName = name;
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

int   ServerConfig::getPort() const
{
    return (_port);
}

const std::string &ServerConfig::getServerName() const
{
    return (_serverName);
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