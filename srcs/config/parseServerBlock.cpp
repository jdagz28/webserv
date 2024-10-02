/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseServerBlock.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 11:19:31 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/02 23:20:09 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "webserv.hpp"
#include <iostream>
#include <fstream>
#include <sstream>


void Config::parseErrorPages(std::istringstream &iss, ServerConfig &serverConfig)
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
        StatusCode error = static_cast<StatusCode>(errorCode);
        serverConfig.setErrorPage(error, errorPagePath);
    }
}

bool validPort(const std::string &value)
{
    int port = strToInt(value);
    if (port < 0 || port > 65535)
        return (false);
    return (true);
}

bool checkAddr(const std::string &host, const std::string &port)
{
    struct addrinfo hints, *res, *p;
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

void Config::parseServerListen(const std::string &value)
{
    std::vector<std::string> check = splitBySpaces(value);
    if (check.size() != 1)
    {
        _error = "Invalid parameter \"" + value + "\"";
        throw configException(_error);
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
        _error = "Invalid port number in \"listen\" directive";
        throw configException(_error);
    }
    if (!checkAddr(host, port))
    {
        _error = "host not found in \"" + host + ":" port + "\"" of the "\"listen\" directive";
        throw configException(_error);
    }
    std::string addr = host + ":" + port;
    serverConfig.setPort(addr);
    _directives["listen"].push_back(value);
}


void Config::parseServerDirective(const std::string &token, std::istringstream &iss, std::ifstream &infile, ServerConfig &serverConfig)
{
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
    else if (token == "error_page")
        parseErrorPages(iss, serverConfig);
    else if (token == "listen" || token == "server_name")
    {
        std::string value;
        std::getline(iss, value);
        trimWhitespaces(value);
        if (value[value.length() - 1]  != ';')
        {
            _error = "Missing semicolon at the end of directive \"" + token + "\".";
            throw configException(_error);
        }
        value = value.substr(0, value.length() - 1);
        if (token == "listen")
            parseServerListen(value);
        else if (token == "server_name")
            parseServerName(value);
    }
    else
    {
        _error = "Invalid directictive in server block.";
        throw configException(_error);
    }
}

void Config::parseServerBlock(std::ifstream &infile, ServerConfig &serverConfig)
{
    std::string line;
    int openingBrace = 0;
    int closingBrace = 0  ;

    while (std::getline(infile, line))
    {
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
        _error = "Mismatch braces for server block in server block";
        throw configException(_error);
    }
}