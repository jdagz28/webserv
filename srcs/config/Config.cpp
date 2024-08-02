/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 22:38:59 by jdagoy            #+#    #+#             */
/*   Updated: 2024/07/08 02:23:45 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "webserv.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

Config::Config(const std::string &configPath)
    : _error(std::make_pair(0, "")), _serverCount(0)
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
        _error = std::make_pair(1, "No configuration file provided.");
        throw configException(_error.second);
    }
    else if (validExtension(configPath) == false)
    {
        _error = std::make_pair(2, "Invalid configuration file type.");
        throw configException(_error.second);
    }
    else if (validPath(configPath) == false)
    {
        _error = std::make_pair(3, "Configuration file is not accessible.");
        throw configException(_error.second);
    }

}

void    Config::parseLocationBlock(std::ifstream &infile, LocationConfig &locationConfig)
{
    std::string line;
    while(std::getline(infile, line))
    {
        if (line.find("}") != std::string::npos)
            break;
        
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (!token.empty())
        {
            std::string directive = token;
            std::string value;
            std::getline(iss, value);
            trimWhitespaces(value);
            if (directive == "root")
                locationConfig.setPath(value);
            if (directive != "{" && directive != "}")
            {
                std::vector<std::string> values = splitBySpaces(value);
                for (size_t i = 0; i < values.size(); i++)
                    locationConfig.setDirective(directive, values[i]);
            }
        }
    }
}

void    Config::parseServerBlock(std::ifstream &infile, ServerConfig &serverConfig)
{
    std::string line;
    while (std::getline(infile, line))
    {
        if (line.find("}") != std::string::npos)
            break;

        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token == "location")
        {
            LocationConfig locationConfig;
            parseLocationBlock(infile, locationConfig);
            serverConfig.setLocationConfig(locationConfig);
        }
        else
        {
            if (!token.empty())
            {
                std::string directive = token;
                std::string value;
                std::getline(iss, value);
                trimWhitespaces(value);
                if (directive == "listen")
                    serverConfig.setPort(value);
                else if (directive == "server_name")
                    serverConfig.setServerName(value);
                if (directive != "{" && directive != "}")
                {
                    std::vector<std::string> values = splitBySpaces(value);
                    if (directive == "error_page")
                    {
                        for (size_t i = 0; i < values.size(); i++)
                        {
                            if (i + 1 < values.size())
                            {
                                std::string errorCode = values[i];
                                std::string errorPage = values[i + 1];
                                serverConfig.setDirective(directive, errorCode + " " + errorPage);
                            }
                            i++;
                        }
                    }
                    else
                        for (size_t i = 0; i < values.size(); i++)
                            serverConfig.setDirective(directive, values[i]);
                }
            }
        }
    }
}

void    Config::parseConfig(const std::string &configFile)
{
    std::ifstream   infile(configFile.c_str());

    if (infile.fail())
    {
        _error = std::make_pair(3, "Configuration file is not accessible.");
        throw configException(_error.second);
    }

    std::string line;
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token == "server")
        {
            ServerConfig    serverConfig;
            parseServerBlock(infile, serverConfig);
            _serverConfig.push_back(serverConfig);
            _serverCount++;
        }
    }
       
}

const std::vector<ServerConfig>& Config::getServerConfig() const
{
    return (_serverConfig);
}

const char *Config::configException::what() const throw()
{
    return (exceptMsg.c_str());
}