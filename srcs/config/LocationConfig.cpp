/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/07 23:05:41 by jdagoy            #+#    #+#             */
/*   Updated: 2024/07/07 02:10:03 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"

LocationConfig::LocationConfig()
    : _path("")
{
}

LocationConfig::LocationConfig(const LocationConfig &copy)
{
    _path = copy.getPath();
    _directives = copy.getDirectives();
}

LocationConfig::~LocationConfig()
{}

LocationConfig    &LocationConfig::operator=(const LocationConfig &copy)
{
    _path = copy.getPath();
    _directives = copy.getDirectives();
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

const std::map<std::string, std::vector<std::string> > &LocationConfig::getDirectives() const
{
    return (_directives);
}

const std::string &LocationConfig::getPath() const
{
    return (_path);
}

