/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseHttpBlock.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/01 11:20:02 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/30 12:53:12 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "webserv.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

void Config::parseKeepAlive(std::istringstream &iss)
{
    std::string value;
    std::getline(iss, value);
    trimWhitespaces(value);
    std::stringstream ss(value);
    if (!value.empty())
    {
        if (value[value.length() - 1] != ';')
        {
            _error = std::string("directive ") + GREEN + "\"keepalive_timeout\" " + RESET
                        + "is not terminated with semicolon " + GREEN + "\";\"" + RESET;
            throw configException(_error);
        }
        if (value[value.length() - 2] == 's')
            value = value.substr(0, value.length() - 2);
        else
            value = value.substr(0, value.length() - 1);
    }
    int timeout;
    ss >> timeout;
    if (ss.fail())
    {
        _error = std::string("invalid ") + GREEN + "\"keepalive_timeout\" " + RESET + value;
        throw configException(_error);
    }
    _keepAliveTimeOut = timeout;
}

bool Config::checkErrorPage(const std::string &errorPagePath)
{
    for (size_t i = 0; i < errorPagePath.length(); i++)
    {
        if (errorPagePath[i] != '/' && errorPagePath[i] != '.' && !std::isalnum(errorPagePath[i]))
            return (false);
    }
    return (true);
}

void Config::parseErrorPages(std::istringstream &iss)
{
    std::string value;
    std::getline(iss, value);
    trimWhitespaces(value);
    std::vector<std::string> values = splitBySpaces(value);
    if (values.empty())
    {
        _error = std::string("empty ") + GREEN + "\"error_page\" " + RESET + "directive";
        throw configException(_error, _configPath, _parsedLine);
    }
    std::string errorPagePath = values.back();
    trimWhitespaces(errorPagePath);
    if (errorPagePath.empty() || errorPagePath[errorPagePath.length() - 1] != ';')
    {
        _error = std::string("directive ") + GREEN + "\"error_page\"" 
                    + RESET + "is not terminated with semicolon " + GREEN + "\";\"" + RESET;
        throw configException(_error, _configPath, _parsedLine);
    }
    errorPagePath = errorPagePath.substr(0, errorPagePath.length() - 1);
    if (!checkErrorPage(errorPagePath))
    {
        _error = std::string("invalid value in ") + GREEN + "\"error_page\"" + RESET + ": " + errorPagePath;
        throw configException(_error, _configPath, _parsedLine);
    }
    for (size_t i = 0; i < values.size() - 1; i++)
    {
        int errorCode = strToInt(values[i]);
        if (errorCode == -1 || getStatusReason(static_cast<StatusCode>(errorCode)) == "Unknown Status Code")
        {
            _error = std::string("invalid HTTP status code in ") + GREEN + "\"error_page\"" + RESET + " directive" ;
            throw configException(_error, _configPath, _parsedLine);
        }
        StatusCode error = static_cast<StatusCode>(errorCode);
        _errorPages[error] = errorPagePath;
    }
}

void Config::parseHttpDirective(const std::string &token, std::istringstream &iss, std::ifstream &infile)
{
    if (token == "keepalive_timeout")
        parseKeepAlive(iss);
    else if (token == "error_page")
        parseErrorPages(iss);
    else if (token == "server")
    {
        std::string check;
        iss >> check;
        if (check != "server" && !check.empty())
        {
            _error = "invalid server block";
            throw configException(_error, _configPath, _parsedLine);
        }
        
        ServerConfig    serverConfig;
        parseServerBlock(infile, serverConfig);
        _serverConfig.push_back(serverConfig);
        _serverCount++;
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

void Config::parseHttpBlock(std::ifstream &infile)
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
        if (token == "{" || token == "}")
            continue ;
        parseHttpDirective(token, iss, infile);
    }
    if (openingBrace != closingBrace)
    {
        _error = "mismatch braces in http bloc.";
        throw configException(_error, _configPath, _parsedLine);
    }
}
