/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/07 23:05:41 by jdagoy            #+#    #+#             */
/*   Updated: 2024/08/08 07:08:01 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"
#include <algorithm>

LocationConfig::LocationConfig()
    : _path(""), _isLimited("False")
{
}

LocationConfig::LocationConfig(const LocationConfig &copy)
{
    _path = copy.getPath();
    _directives = copy.getDirectives();
    _allowedMethods = copy.getAllowedMethods();
    _isLimited = copy.isLimited();
    _limitExcept = copy.getLimitExcept();
}

LocationConfig::~LocationConfig()
{}

LocationConfig    &LocationConfig::operator=(const LocationConfig &copy)
{
    _path = copy.getPath();
    _directives = copy.getDirectives();
    _allowedMethods = copy.getAllowedMethods();
    _isLimited = copy.isLimited();
    _limitExcept = copy.getLimitExcept();
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

void    LocationConfig::setLimitExcept(bool limited)
{
    _isLimited = limited;
}

void    LocationConfig::setExcludeMethod(const std::string &method)
{
    _limitExcept.push_back(method);
}

const std::map<std::string, std::vector<std::string> > &LocationConfig::getDirectives() const
{
    return (_directives);
}

const std::string &LocationConfig::getPath() const
{
    return (_path);
}

const std::vector<std::string> &LocationConfig::getAllowedMethods() const
{
    return(_allowedMethods);
}

const std::vector<std::string> &LocationConfig::getLimitExcept() const
{
    return(_limitExcept);
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

bool LocationConfig::isLimited() const
{
    return (_isLimited);
}

bool LocationConfig::isMethodExcluded(const std::string &method) const
{
    if (_limitExcept.empty())
        return (false);
    std::vector<std::string>::const_iterator it;
    for(it = _limitExcept.begin(); it != _limitExcept.end(); it++)
        if (*it == method)
            return (true);
    return (false);
}
