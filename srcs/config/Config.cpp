/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 22:38:59 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/23 00:06:32 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "webserv.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

Config::Config(const std::string &configPath)
    : _configPath(configPath), _parsedLine(0), _error(""), _serverCount(0), _keepAliveTimeOut(0)
{
    if (configPath != DEFAULT_CONFIG)
        checkFile(configPath);
    parseConfig(configPath);
}

Config::Config(const Config &copy)
{
    _configPath = copy._configPath;
    _parsedLine = copy._parsedLine;
    _error = copy._error;
    _serverConfig = copy._serverConfig;
    _serverCount = copy._serverCount;
    _keepAliveTimeOut = copy._keepAliveTimeOut;
    _errorPages = copy._errorPages;
}

Config::~Config()
{}

Config  &Config::operator=(const Config &copy)
{
    if (this != &copy)
    {
        _configPath = copy._configPath;
        _parsedLine = copy._parsedLine;
        _error = copy._error;
        _serverConfig = copy._serverConfig;
        _serverCount = copy._serverCount;
        _keepAliveTimeOut = copy._keepAliveTimeOut;
        _errorPages = copy._errorPages;
    }
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
        _error = "no configuration file provided.";
        throw configException(_error);
    }
    else if (validExtension(configPath) == false)
    {
        _error = "invalid configuration file type.";
        throw configException(_error);
    }
    else if (validPath(configPath) == false)
    {
        _error = "configuration file is not accessible.";
        throw configException(_error);
    }

}

void Config::skipEventsBlock(std::ifstream &infile)
{
    std::string line;
    int openingBrace = 0;
    int closingBrace = 0;

    while (std::getline(infile, line))
    {
        _parsedLine++;
        trimWhitespaces(line);
        if (line.empty() || line[0] == '#')
            continue ;

        std::istringstream iss(line);
        std::string token;
        iss >> token;
        
        checkBraces(token, openingBrace, closingBrace);
        if (token == "{")
            continue ;
        if (token == "}")
            break ; 
    }
    if (openingBrace != closingBrace)
    {
        _error = "mismatch braces in server block";
        throw configException(_error, _configPath, _parsedLine);
    }
}

void    Config::checkBraces(const std::string &token, int &openingBrace, int &closingBrace)
{
    if (token == "{")
        openingBrace++;
    else if (token == "}")
        closingBrace++;
    if (closingBrace > 1 || openingBrace > 1)
    {
        _error = std::string("unexpected ") + GREEN + "\"" + token + "\"" + RESET;
        throw configException(_error, _configPath, _parsedLine);
    }
    if (closingBrace > openingBrace)
    {
        _error = "a closing brace without an opening brace in http block";
        throw configException(_error, _configPath, _parsedLine);
    }
}


void    Config::parseConfig(const std::string &configFile)
{
    std::ifstream   infile(configFile.c_str());

    if (infile.fail())
    {
        _error = "configuration file is not accessible";
        throw configException(_error, _configPath, _parsedLine);
    }

    std::string line;
    
    while (std::getline(infile, line))
    {
        _parsedLine++;
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
            _error = "server block outside of http block";
            throw configException(_error, _configPath, _parsedLine);
        }
        else if (token == "location")
        {
            _error = "location block outside of server block";
            throw configException(_error, _configPath, _parsedLine);
        }
        else
        {
            _error = "invalid directive in http block";
            throw configException(_error, _configPath, _parsedLine);
        }
    }
}

const std::vector<ServerConfig>& Config::getServerConfig() const
{
    return (_serverConfig);
}

const char *Config::configException::what() const throw()
{
    if (!configPath.empty() && !parsedLine.empty())
    {
        errorMsg = RED + std::string("webserv: ") + RESET + "[emerg] " + exceptMsg
                    + " in " + RESET + configPath + ":" + RED + parsedLine + std::string(RESET);
        return (errorMsg.c_str());
    }
    else
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