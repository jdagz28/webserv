/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseHttpBlock.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/01 11:20:02 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/01 12:44:31 by jdagoy           ###   ########.fr       */
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
            _error = "directive \"keepalive_timeout\" is not terminated with semicolon \";\"";
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
        _error = "Invalid keepalive_timeout value.";
        throw configException(_error);
    }
    _keepAliveTimeOut = timeout;
}

bool checkErrorPage(const std::string &errorPagePath)
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
        _error = "Empty \"error_page\" directive.";
        throw configException(_error);
    }
    std::string errorPagePath = values.back();
    trimWhitespaces(errorPagePath);
    if (errorPagePath.empty() || errorPagePath[errorPagePath.length() - 1] != ';')
    {
        _error = "directive \"error_page\" is not terminated with semicolon \";\"";
        throw configException(_error);
    }
    errorPagePath = errorPagePath.substr(0, errorPagePath.length() - 1);
    if (!checkErrorPage(errorPagePath))
    {
        _error = "Invalid value in \"error_page\": " + errorPagePath;
        throw configException(_error);
    }
    for (size_t i = 0; i < values.size() - 1; i++)
    {
        int errorCode = strToInt(values[i]);
        if (errorCode == -1 || getStatusReason(static_cast<StatusCode>(errorCode)) == "Unknown Status Code")
        {
            _error = "Invalid HTTP status code in \"error_page\" directive" ;
            throw configException(_error);
        }
        _errorPages[errorCode] = errorPagePath;
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
        ServerConfig    serverConfig;
        parseServerBlock(infile, serverConfig);
        _serverConfig.push_back(serverConfig);
        _serverCount++;
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

void Config::parseHttpBlock(std::ifstream &infile)
{
    std::string line;
    int braceCount = 0;
    bool hasOpeningBrace = false;
    bool hasClosingBrace = false;
    
    while (std::getline(infile, line))
    {
        trimWhitespaces(line);
        if (line.empty() || line[0] == '#')
            continue ;
        
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        checkBraces(token, braceCount, hasOpeningBrace, hasClosingBrace);
        if (token == "{" || token == "}")
            continue ;

        parseHttpDirective(token, iss, infile);
    }
    if (braceCount > 2 || (braceCount == 2 && !hasOpeningBrace) || (braceCount == 2 && !hasClosingBrace))
    {
        _error = "Mismatch braces for http block.";
        throw configException(_error);
    }
}
