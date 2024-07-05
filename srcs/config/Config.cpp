/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 22:38:59 by jdagoy            #+#    #+#             */
/*   Updated: 2024/07/05 22:27:31 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <iostream>
#include <fstream>

Config::Config(const std::string &configPath)
    : error(std::make_pair(0, ""))
{
    try
    {
        if (configPath != DEFAULT_CONFIG)
            checkFile(configPath);
        parseConfig(configPath);        
    }
    catch (const configException &e)
    {
        std::cerr << e.what() << std::endl;
    } 
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

bool    Config::validPath(const std::string &configPath)
{
    std::ifstream   infile;

    infile.open(configPath.c_str());
    if (!infile.is_open())
        return (false);
    return (true);
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
        error = std::make_pair(1, "No configuration file provided.");
        throw configException(error.second);
    }
    else if (validExtension(configPath) == false)
    {
        error = std::make_pair(2, "Invalid configuration file type.");
        throw configException(error.second);
    }
    else if (validPath(configPath) == false)
    {
        error = std::make_pair(3, "Configuration file is not accessible.");
        throw configException(error.second);
    }

}

bool    Config::isError() const
{
    if (error.first)
        return (true);
    return (false);
}

/**
    * parseConfig flow (with map)
    * - parse the whole config 
    * - parse per block: server, http, location (in their own class?)
*/

void    Config::parseConfig(const std::string &configFile)
{
    std::ifstream   infile(configFile);

    if (infile.fail())
    {
        error = std::make_pair(3, "Configuration file is not accessible.");
        throw configException(error.second);
    }
       
}


const char *Config::configException::what() const throw()
{
    return (exceptMsg.c_str());
}