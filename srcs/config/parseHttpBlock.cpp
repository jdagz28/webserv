/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseHttpBlock.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/01 11:20:02 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/01 11:23:39 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "webserv.hpp"
#include <iostream>
#include <fstream>
#include <sstream>


void    Config::parseHttpBlock(std::ifstream &infile)
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
        else if (token == "error_page")
        {
            std::string value;
            std::getline(iss, value);
            trimWhitespaces(value);
            std::vector<std::string> values = splitBySpaces(value);
            std::string errorPagePath = values[values.size() - 1];
            for (size_t i = 0; i < values.size() - 1; i++)
            {
                int errorCode = strToInt(values[i]);
                if (errorCode == -1)
                {
                    _error = "Invalid error_page directive.";
                    throw configException(_error);
                }
                _errorPages[errorCode] = errorPagePath;
            }
        }
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
    
    if (braceCount > 2 || (braceCount == 2 && !hasOpeningBrace) || (braceCount == 2 && !hasClosingBrace))
    {
        _error = "Mismatch braces for http block.";
        throw configException(_error);
    }
}
