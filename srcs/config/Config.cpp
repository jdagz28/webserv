/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 22:38:59 by jdagoy            #+#    #+#             */
/*   Updated: 2024/07/05 03:35:14 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <iostream>

Config::Config(const std::string &configPath)
    : error(false), errorMsg("")
{
    if (configPath != DEFAULT_CONFIG)
        checkFile(configPath);
    if (isError()) //!
        std::cerr << errorMsg << std::endl;
}

Config::Config(const Config &copy)
{
    (void) copy;
}

Config::~Config()
{}

Config  &Config::operator=(const Config &copy)
{
    (void) copy;
    return (*this);
}

bool    Config::validExtension(const std::string &configPath)
{
    if (configPath.length() < 5)
        return (false);
    else
    {
        std::string check = configPath.substr(configPath.length() - 5);
        if (check != ".conf")
            return (false);
    }
    return (true);
}


void    Config::checkFile(const std::string &configPath)
{
    if (configPath.empty())
    {
        error = true;
        errorMsg = "No configuration file provided.";
        return ;
    }
    if (validExtension(configPath) == false)
    {
        error = true;
        errorMsg = "Invalid configuration file type.";
        return ;
    }
    
}

bool    Config::isError() const
{
    return (error);
}