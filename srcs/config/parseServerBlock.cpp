/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseServerBlock.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 11:19:31 by jdagoy            #+#    #+#             */
/*   Updated: 2025/03/09 22:59:57 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "webserv.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <netdb.h>

void	Config::parseErrorPages(std::istringstream &iss, ServerConfig &serverConfig)
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

bool	Config::checkServerName(const std::string &name)
{
    for (size_t i = 0; i < name.length(); i++)
    {
        if (!std::isalnum(name[i]) && name[i] != '.')
            return (false);
    }
    return (true);
}

void	Config::parseServerName(const std::string &value, ServerConfig &serverConfig)
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

bool	Config::validPort(const std::string &value)
{
    int port = strToInt(value);
    if (port < 0 || port > 65535)
        return (false);
    return (true);
}

bool	Config::checkAddr(const std::string &host, const std::string &port)
{
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;      
    hints.ai_socktype = SOCK_STREAM;
    
    int status = getaddrinfo(host.c_str(), port.c_str(),&hints, &res);
    if (status != 0)
    {
        if (res)
            freeaddrinfo(res);
        return (false);
    }
    freeaddrinfo(res);
    return (true);
}

void	Config::parseServerListen(const std::string &value, ServerConfig &serverConfig)
{
    std::vector<std::string> check = splitBySpaces(value);
    if (check.size() > 3)
    {
        _error = std::string("invalid parameter ") + GREEN + "\"" + value + "\"" + RESET;
        throw configException(_error, _configPath, _parsedLine);
    }

	for (size_t i = 0; i < check.size(); i++)
	{
		std::string port;
		std::string ip;
		
		if (check[i].find(':') != std::string::npos)
		{
			size_t colonPos = check[i].find(':');
			ip = check[i].substr(0, colonPos);
			port = check[i].substr(colonPos + 1);
		}
		else
			port = check[i];
			
		if (!validPort(port))
		{
			_error = std::string("Invalid port number in ") + GREEN + "\"listen\"" + RESET + " directive";
			throw configException(_error, _configPath, _parsedLine);
		}
		serverConfig.setPort(strToInt(port));
		_portsToServe.push_back(strToInt(port));

		if (!ip.empty())
		{
			if (!checkAddr(ip, port))
			{
				_error = std::string("Invalid IP address in ") + GREEN + "\"listen\"" + RESET + " directive";
				throw configException(_error, _configPath, _parsedLine);
			}
			serverConfig.setIP(ip);
		}
		else
			serverConfig.setIP(LOCALHOST);
	}
	serverConfig.setDirective("listen", value);
    
}

void	Config::parseServerDirective(const std::string &token, std::istringstream &iss, std::ifstream &infile, ServerConfig &serverConfig)
{
    if (token == "location")
    {
        LocationConfig locationConfig;
        std::string value;
        std::getline(iss, value);
        trimWhitespaces(value);

        if (value.empty())
        {
            _error = std::string("empty ") + GREEN + "\"location\" " + RESET + "path";
            throw configException(_error, _configPath, _parsedLine);
        }
        if (serverConfig.isPathAlreadySet(value))
        {
            _error = std::string("duplicate location ") + GREEN + "\"" + value + "\"" + RESET;
            throw configException(_error, _configPath, _parsedLine);
        }
		if (value[0] == '.')
		{
			parseExtensionLocation(infile, locationConfig);
			locationConfig.setCGIExtension(value);
			locationConfig.setDirective("cgi_extension", value);
		}
		else
        	parseLocationBlock(infile, locationConfig);
        if (value[value.length() - 2] == '/' && value[value.length() - 1] == '*')
            value = value.substr(0, value.length() - 2);
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
        _error = "invalid directive in server block";
        throw configException(_error, _configPath, _parsedLine);
    }
	if (serverConfig.getServerNames().size() == 0)
		serverConfig.setServerName("localhost");
}

void	Config::parseServerBlock(std::ifstream &infile, ServerConfig &serverConfig)
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