/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 02:19:46 by jdagoy            #+#    #+#             */
/*   Updated: 2024/07/07 02:12:47 by jdagoy           ###   ########.fr       */
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