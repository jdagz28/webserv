/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 22:38:59 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/30 22:54:43 by jdagoy           ###   ########.fr       */
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

void    Config::parseServerBlock(std::ifstream &infile, ServerConfig &serverConfig)
{
    std::string line;
    while (std::getline(infile, line))
    {
        if (line.find("}") != std::string::npos)
            break;

        // std::cout << "Line: " << line << std::endl;
        std::istringstream iss(line);
        std::string token;
        // std::cout << "Token: " << token << std::endl;
        iss >> token;
        if (token == "location")
        {
            LocationConfig locationConfig;
            parseLocationBlock(infile, locationConfig);
            std::string value;
            std::getline(iss, value);
            trimWhitespaces(value);
            locationConfig.setPath(value);
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

void Config::skipEventsBlock(std::ifstream &infile)
{
    std::string line;
    int braceCount = 0;
    bool hasOpeningBrace = false;

    while (std::getline(infile, line))
    {
        size_t posOpen = line.find("{");
        if (posOpen != std::string::npos)
        {
            braceCount++;
            hasOpeningBrace = true;
        }
        size_t posClose = line.find("}");
        if (posClose != std::string::npos)
        {
            braceCount--;
            if (braceCount < 0)
            {
                _error = "No opening brace for events block.";
                throw configException(_error);
            }
        }
        if (braceCount == 1)
        {
            _error = "No closing brace for events block.";
            throw configException(_error);
        }
        if (posOpen > posClose)
        {
            _error = "Mismatch braces for events block.";
            throw configException(_error);
        }
        if (braceCount == 0 && hasOpeningBrace)
            return ;
    }
    if (braceCount != 0)
    {
        _error = "Mismatch braces for events block.";
        throw configException(_error);
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
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token == "events")
        {
            skipEventsBlock(infile);
            continue ;
        }
        if (token == "keepalive_timeout")
        {
            std::string value;
            std::getline(iss, value);
            trimWhitespaces(value);
            std::stringstream ss(value);
            int timeout;
            ss >> timeout;
            if (ss.fail())
            {
                _error = "Invalid keepalive_timeout value.";
                throw configException(_error);
            }
            _keepAliveTimeOut = timeout;
        }
        else if (token == "server")
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

time_t  Config::getKeepAliveTimeout() const
{
    return (_keepAliveTimeOut);
}