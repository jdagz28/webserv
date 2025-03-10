/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 02:19:46 by jdagoy            #+#    #+#             */
/*   Updated: 2025/01/07 15:53:49 by jdagoy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"
#include <cstdlib>

ServerConfig::ServerConfig()
    : _port(-1), _valid(false)
{}

ServerConfig::ServerConfig(const ServerConfig &copy)
    : _directives(copy._directives), 
		_ip(copy._ip), _port(copy._port), 
		_serverName(copy._serverName), 
		_locationConfig(copy._locationConfig), 
		_errorPages(copy._errorPages), 
		_locationPaths(copy._locationPaths), 
		_valid(copy._valid)
{}

ServerConfig::~ServerConfig()
{}

ServerConfig	&ServerConfig::operator=(const ServerConfig &copy)
{
    if (this != &copy)
    {
        _directives = copy._directives;
		_ip = copy._ip;
        _port = copy._port;
        _serverName = copy._serverName;
        _locationConfig = copy._locationConfig;
        _errorPages = copy._errorPages;
        _locationPaths = copy._locationPaths;
        _valid = copy._valid;
    }
    return (*this);
}

void   ServerConfig::setDirective(const std::string &directive, const std::string &value)
{
    _directives[directive].push_back(value);
}

void    ServerConfig::setPort(int port)
{
    _port = port;
}

void    ServerConfig::setIP(const std::string &ip)
{
    _ip = ip;
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

void    ServerConfig::setValid()
{
    _valid = true;
}

const std::map<std::string, std::vector<std::string> >	&ServerConfig::getDirectives() const
{
    return (_directives);
}

int	ServerConfig::getPort() const
{
    return (_port);
}

const std::string	ServerConfig::getIP() const
{
    return (_ip);
}

std::vector<std::string>	ServerConfig::getServerNames() const
{
    return (_serverName);
}

bool    ServerConfig::isValid() const
{
    return (_valid);
}

std::string	ServerConfig::checkServerName(const std::string &requestHost) const
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

const std::vector<LocationConfig>	&ServerConfig::getLocationConfig() const
{
    return (_locationConfig);
}

LocationConfig ServerConfig::getLocationConfig(const std::string &path) const
{
    std::vector<LocationConfig>::const_iterator it;
    for (it = _locationConfig.begin(); it != _locationConfig.end(); it++)
    {
        if (it->getPath() == path)
            return (*it);
    }
    return (LocationConfig());   
}

const std::string	ServerConfig::getErrorPage(int status) const
{    
    std::map<int, std::string>::const_iterator it;
    for (it = _errorPages.begin(); it != _errorPages.end(); it++)
    {
        if (it->first == status)
            return (it->second);
    }

    return (std::string());
}

void	ServerConfig::setLocationPath(const std::string &path)
{
    _locationPaths.push_back(path);
}

bool	ServerConfig::isPathAlreadySet(const std::string &path) const
{
    std::vector<std::string>::const_iterator it;
    for (it = _locationPaths.begin(); it != _locationPaths.end(); it++)
    {
        if (*it == path)
            return (true);
    }
    return (false);
}