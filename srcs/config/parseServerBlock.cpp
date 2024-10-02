/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseServerBlock.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 11:19:31 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/02 15:48:19 by jdagoy           ###   ########.fr       */
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
    else
    {
        if (!token.empty())
        {
            std::string directive = token;
            std::string value;
            std::getline(iss, value);
            trimWhitespaces(value);
            if (value[value.length() - 1]  != ';')
            {
                _error = "Missing semicolon at the end of directive \"" + directive + "\".";
                throw configException(_error);
            }
            value = value.substr(0, value.length() - 1);
            if (directive == "listen")
                serverConfig.setPort(value);
            else if (directive == "server_name")
                serverConfig.setServerName(value);
        }
    }
}

void Config::parseServerBlock(std::ifstream &infile, ServerConfig &serverConfig)
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

        parseServerDirective(token, iss, infile, serverConfig);
    }
    if (braceCount > 2 || (braceCount == 2 && !hasOpeningBrace) || (braceCount == 2 && !hasClosingBrace))
    {
        _error = "Mismatch braces for server block.";
        throw configException(_error);
    }
}