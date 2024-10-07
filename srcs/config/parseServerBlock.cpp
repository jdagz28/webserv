/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseServerBlock.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 11:19:31 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/07 13:29:58 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "webserv.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <netdb.h>

void Config::parseErrorPages(std::istringstream &iss, ServerConfig &serverConfig)
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
            _error = std::string("invalid HTTP status code in ") + GREEN + "\"error_page\" " + RESET + " directive" ;
            throw configException(_error, _configPath, _parsedLine);
        }
        StatusCode error = static_cast<StatusCode>(errorCode);
        serverConfig.setErrorPage(error, errorPagePath);
    }
}

bool Config::checkServerName(const std::string &name)
{
    for (size_t i = 0; i < name.length(); i++)
    {
        if (!std::isalnum(name[i]) && name[i] != '.')
            return (false);
    }
    return (true);
}

void Config::parseServerName(const std::string &value, ServerConfig &serverConfig)
{
    std::vector<std::string> serverNames = splitBySpaces(value);
    for (size_t i = 0; i < serverNames.size(); i++)
    {
        if (!checkServerName(serverNames[i]))
        {
            _error = "invalid server name in \"server_name\" directive";
            throw configException(_error, _configPath, _parsedLine);
        }
        serverConfig.setServerName(serverNames[i]);
    }
    serverConfig.setDirective("server_name", value);
}

bool Config::validPort(const std::string &value)
{
    int port = strToInt(value);
    if (port < 0 || port > 65535)
        return (false);
    return (true);
}

bool Config::checkAddr(const std::string &host, const std::string &port)
{
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;      
    hints.ai_socktype = SOCK_STREAM;
    
    int status = getaddrinfo(host.c_str(), port.c_str(),&hints, &res);
    if (status == -1)
    {
        freeaddrinfo(res);
        return (false);
    }
    freeaddrinfo(res);
    return (true);
}

void Config::parseServerListen(const std::string &value, ServerConfig &serverConfig)
{
    std::vector<std::string> check = splitBySpaces(value);
    if (check.size() != 1)
    {
        _error = std::string("invalid parameter ") + GREEN + "\"" + value + "\"" + RESET;
        throw configException(_error, _configPath, _parsedLine);
    }
    
    size_t colonPos = value.find(':');
    std::string host;
    std::string port;
    if (colonPos != std::string::npos)
    {
        host = value.substr(0, colonPos);
        port = value.substr(colonPos + 1);
    }
    else
    {
        host = "0.0.0.0";
        port = value;
    }
    
    if (!validPort(port))
    {
        _error = std::string("Invalid port number in ") + GREEN + "\"listen\"" + RESET + " directive";
        throw configException(_error, _configPath, _parsedLine);
    }
    if (!checkAddr(host, port))
    {
        _error = std::string("host not found in \"") + host + ":" + port + "\" of the " 
                    + GREEN + "\"listen\"" + RESET + " directive";
        throw configException(_error, _configPath, _parsedLine);
    }
    std::string addr = host + ":" + port;
    serverConfig.setPort(addr);
    serverConfig.setDirective("listen", value);
}

void Config::parseServerDirective(const std::string &token, std::istringstream &iss, std::ifstream &infile, ServerConfig &serverConfig)
{
    if (token == "location")
    {
        LocationConfig locationConfig;
        std::string value;
        std::getline(iss, value);
        trimWhitespaces(value);
        if (serverConfig.isPathAlreadySet(value))
        {
            _error = std::string("duplicate location ") + GREEN + "\"" + value + "\"" + RESET;
            throw configException(_error, _configPath, _parsedLine);
        }
        parseLocationBlock(infile, locationConfig);
        locationConfig.setPath(value);
        serverConfig.setLocationConfig(locationConfig);
        serverConfig.setLocationPath(value);
    }
    else if (token == "error_page")
        parseErrorPages(iss, serverConfig);
    else if (token == "listen" || token == "server_name")
    {
        std::string value;
        std::getline(iss, value);
        trimWhitespaces(value);
        if (value[value.length() - 1]  != ';')
        {
            _error = std::string("missing semicolon at the end of directive ") + GREEN + "\"" + token + "\"" + RESET;
            throw configException(_error, _configPath, _parsedLine);
        }
        value = value.substr(0, value.length() - 1);
        if (token == "listen")
            parseServerListen(value, serverConfig);
        else if (token == "server_name")
            parseServerName(value, serverConfig);
    }
    else
    {
        _error = "invalid directictive in server block";
        throw configException(_error, _configPath, _parsedLine);
    }
}

void Config::parseServerBlock(std::ifstream &infile, ServerConfig &serverConfig)
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
        parseServerDirective(token, iss, infile, serverConfig);
    }
    if (openingBrace != closingBrace)
    {
        _error = "mismatch braces in server block";
        throw configException(_error, _configPath, _parsedLine);
    }
}