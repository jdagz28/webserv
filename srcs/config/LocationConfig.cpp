/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/07 23:05:41 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/07 13:22:50 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"
#include <algorithm>

LocationConfig::LocationConfig()
    : _path("")
{
}

LocationConfig::LocationConfig(const LocationConfig &copy)
{
    _path = copy.getPath();
    _directives = copy.getDirectives();
    _allowedMethods = copy.getAllowedMethods();
}

LocationConfig::~LocationConfig()
{}

LocationConfig    &LocationConfig::operator=(const LocationConfig &copy)
{
    if (this != &copy)
    {
        _path = copy.getPath();
        _directives = copy.getDirectives();
        _allowedMethods = copy.getAllowedMethods();
    }
    return (*this);
}

void   LocationConfig::setDirective(const std::string &directive, const std::string &value)
{
    _directives[directive].push_back(value);
}

void    LocationConfig::setPath(const std::string &path)
{
    _path = path;
}

void    LocationConfig::setAllowedMethod(const std::string &method)
{
    _allowedMethods.push_back(method);
}

const std::map<std::string, std::vector<std::string> > &LocationConfig::getDirectives() const
{
    return (_directives);
}

const std::string &LocationConfig::getPath() const
{
    return (_path);
}

const std::string LocationConfig::getRoot() const
{
    std::map<std::string, std::vector<std::string> >::const_iterator it;
    for (it = _directives.begin(); it != _directives.end(); it++)
    {
        if (it->first == "root")
            return (it->second[0]);
    }
    return (std::string());
}

const std::vector<std::string> &LocationConfig::getAllowedMethods() const
{
    return(_allowedMethods);
}

const std::string LocationConfig::getDefaultName() const
{
    std::map<std::string, std::vector<std::string> >::const_iterator directive;
    for (directive = _directives.begin(); directive != _directives.end(); directive++)
    {
        if (directive->first == "default")
            return (directive->second[0]);
    }
    return (std::string()); 
}

const std::string LocationConfig::getIndex() const
{
    std::map<std::string, std::vector<std::string> >::const_iterator directive;
    for (directive = _directives.begin(); directive != _directives.end(); directive++)
    {
        if (directive->first == "index")
            return (directive->second[0]);
    }
    return (std::string()); 
}

const std::string LocationConfig::getAutoIndex() const
{
    std::map<std::string, std::vector<std::string> >::const_iterator directive;
    for (directive = _directives.begin(); directive != _directives.end(); directive++)
    {
        if (directive->first == "autoindex")
            return (directive->second[0]);
    }
    return (std::string()); 
}

bool LocationConfig::isMethodAllowed(const std::string &method) const
{
    if (_allowedMethods.empty())
        return (true);
    std::vector<std::string>::const_iterator it;
    for(it = _allowedMethods.begin(); it != _allowedMethods.end(); it++)
        if (*it == method)
            return (true);
    return (false);
}

bool LocationConfig::isRedirect() const
{
    if (_directives.empty())
        return (false);
    std::map<std::string, std::vector<std::string> >::const_iterator directive;
    for (directive = _directives.begin(); directive != _directives.end(); directive++)
    {
        if (directive->first == "return")
            return (true);
    }
    return (false);
}

const std::vector<std::string>& LocationConfig::getRedirect() const
{
    std::map<std::string, std::vector<std::string> >::const_iterator directive;
    for (directive = _directives.begin(); directive != _directives.end(); directive++)
    {
        if (directive->first == "return")
            return (directive->second);
    }
    static const std::vector<std::string> empty;
    return (empty);
}

