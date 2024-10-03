/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 02:19:46 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/03 14:52:46 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"
#include <cstdlib>

ServerConfig::ServerConfig()
{
}

ServerConfig::ServerConfig(const ServerConfig &copy)
{
    // _port = copy.getPort();
    // _serverName = copy.getServerName();
    _locationConfig = copy.getLocationConfig();
    _directives = copy.getDirectives();
}

ServerConfig::~ServerConfig()
{}

ServerConfig    &ServerConfig::operator=(const ServerConfig &copy)
{
    // _port = copy.getPort();
    // _serverName = copy.getServerName();
    _locationConfig = copy.getLocationConfig();
    _directives = copy.getDirectives();
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

// void    ServerConfig::setServerName(const std::string &name)
// {
//     _serverName = name;
// }

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

// int   ServerConfig::getPort() const
// {
//     return (_port);
// }

// const std::string &ServerConfig::getServerName() const
// {
//     return (_serverName);
// }

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