/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 22:38:59 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/02 15:54:00 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "webserv.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

Config::Config(const std::string &configPath)
    : _error(""), _serverCount(0), _keepAliveTimeOut(0)
{
    if (configPath != DEFAULT_CONFIG)
        checkFile(configPath);
    parseConfig(configPath);
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
        _error = "No configuration file provided.";
        throw configException(_error);
    }
    else if (validExtension(configPath) == false)
    {
        _error = "Invalid configuration file type.";
        throw configException(_error);
    }
    else if (validPath(configPath) == false)
    {
        _error = "Configuration file is not accessible.";
        throw configException(_error);
    }

}

void    Config::parseLocationBlock(std::ifstream &infile, LocationConfig &locationConfig)
{
    std::string line;
    while(std::getline(infile, line))
    {
        if (line.find("}") != std::string::npos)
            break;
        // std::cout << "Line: " << line << std::endl;
        std::istringstream iss(line);
        std::string token;
        // std::cout << "Token: " << token << std::endl;
        iss >> token;
        if (!token.empty())
        {
            std::string directive = token;
            std::string value;
            std::getline(iss, value);
            trimWhitespaces(value);
            if (directive == "allowed_methods")
            {
                std::vector<std::string> methods = splitBySpaces(value);
                for (size_t i = 0; i < methods.size(); i++)
                    locationConfig.setAllowedMethod(methods[i]);
            }
            else if (directive == "limit_except")
            {
                locationConfig.setLimitExcept(true);
                std::vector<std::string> methods = splitBySpaces(value);
                for (size_t i = 0; i < methods.size(); i++)
                    locationConfig.setLimitExcept(methods[i]);
            }
            else if (directive != "{" && directive != "}")
            {
                std::vector<std::string> values = splitBySpaces(value);
                for (size_t i = 0; i < values.size(); i++)
                    locationConfig.setDirective(directive, values[i]);
            }
        }
    }
}

void Config::skipEventsBlock(std::ifstream &infile)
{
    std::string line;
    int braceCount = 0;
    bool hasOpeningBrace = false;

    while (std::getline(infile, line))
    {
        trimWhitespaces(line);
        if (line.empty() || line[0] == '#')
            continue ;
        size_t posOpen = line.find("{");
        if (posOpen != std::string::npos)
        {
            braceCount++;
            hasOpeningBrace = true;
        }
        size_t posClose = line.find("}");
        if (posClose != std::string::npos)
        {
            braceCount++;
            if (braceCount == 1)
            {
                _error = "No opening brace for events block.";
                throw configException(_error);
            }
        }
        if (braceCount != 2 || posOpen > posClose)
        {
            _error = "Mismatch braces for events block.";
            throw configException(_error);
        }
        if (braceCount == 2 && hasOpeningBrace)
            return ;
    }
    if (braceCount != 0)
    {
        _error = "Mismatch braces for events block.";
        throw configException(_error);
    }
}

void    Config::checkBraces(const std::string &token, int &braceCount, bool &hasOpeningBrace, bool &hasClosingBrace)
{
    if (token == "{")
    {
        braceCount++;
        hasOpeningBrace = true;
    }
    else if (token == "}")
    {
        braceCount++;
        hasClosingBrace = true;
        if (braceCount == 1 && !hasOpeningBrace)
        {
            _error = "No opening brace for http block.";
            throw configException(_error);
        }
    }
}


void    Config::parseConfig(const std::string &configFile)
{
    std::ifstream   infile(configFile.c_str());

    if (infile.fail())
    {
        _error = "Configuration file is not accessible.";
        throw configException(_error);
    }

    std::string line;
    
    while (std::getline(infile, line))
    {
        trimWhitespaces(line);
        if (line.empty() || line[0] == '#')
            continue ;
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        
        if (token == "events")
        {
            skipEventsBlock(infile);
            continue ;
        }
        else if (token == "http")
        {
            parseHttpBlock(infile);
            break ;
        }
        else if (token == "server")
        {
            _error = "Server block outside of http block.";
            throw configException(_error);
        }
        else if (token == "location")
        {
            _error = "Location block outside of server block.";
            throw configException(_error);
        }
        else
        {
            _error = "Invalid directive in http block.";
            throw configException(_error);
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

time_t  Config::getKeepAliveTimeout() const
{
    return (_keepAliveTimeOut);
}

std::string Config::getErrorPages() const
{
    std::string errorPages;
    std::map<StatusCode, std::string>::const_iterator it;
    for (it = _errorPages.begin(); it != _errorPages.end(); it++)
    {
        errorPages += "Error Code: " + toString(it->first) + " Error Page: " + it->second + "\n";
    }
    return (errorPages);
}