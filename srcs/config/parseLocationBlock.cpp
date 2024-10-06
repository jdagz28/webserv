/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseLocationBlock.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/04 14:50:02 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/07 01:19:54 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "webserv.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>


void Config::parseRedirect(const std::string &value, LocationConfig &locationConfig)
{
    std::vector<std::string> validRedirects{"301", "302", "303", "307", "308"};
    
    std::vector<std::string> values = splitBySpaces(value);
    if (values != 2)
    {
        _error = std::string("invalid number of arguments in ") + GREEN + "\"return\"" + RESET + " directive";
        throw configException(_error, _configPath, _parsedLine);
    }
    if (std::find(validRedirects.begin(), validRedirects.end(), values[0]) == validRedirects.end())
    {
        _error = std::string("invalid value in ") + GREEN + "\"return\"" + RESET + " directive";
        throw configException(_error, _configPath, _parsedLine);
    }
    locationConfig.setDirective("return", value);
}

void Config::parseAutoindex(const std::string &value, LocationConfig &locationConfig)
{
    if (value != "on" && value != "off")
    {
        _error = std::string("invalid value in ") + GREEN + "\"autoindex\"" + RESET + " directive";
        throw configException(_error, _configPath, _parsedLine);
    }
    locationConfig.setDirective("autoindex", value);
}

void Config::parseIndex(const std::string &value, LocationConfig &locationConfig)
{
    for (size_t i = 0; i < value.size(); i++)
    {
        if (!std::isalnum(value[i]) && value[i] != '.')
        {
            _error = std::string("invalid value in ") + GREEN + "\"index\"" + RESET + " directive";
            throw configException(_error, _configPath, _parsedLine);
        }
    }
    locationConfig.setDirective("index", value);
}


void Config::parseRoot(const std::string &value, LocationConfig &locationConfig)
{
    for (size_t i = 0; i < value.size(); i++)
    {
        if (!std::isalnum(value[i]) && value[i] != '/')
        {
            _error = std::string("invalid value in ") + GREEN + "\"root\"" + RESET + " directive";
            throw configException(_error, _configPath, _parsedLine);
        }
    }
    locationConfig.setDirective("root", value);
}

void Config::checkValueNum(const std::string &token, const std::string &value)
{
    if (token == "return" || token == "limit_except" || token == "types")
        return ;
    std::vector<std::string> values = splitBySpaces(value);
    if (value.empty() || values.size() != 1)
    {
        _error = std::string("invalid number of arguments in ") + GREEN + "\"" + token + "\"" + RESET + " directive";
        throw configException(_error, _configPath, _parsedLine);
    }
}

void Config::validLocationDirective(const std::string &token)
{
    if (token == "root" || token == "index" || token == "autoindex" || token == "return" || token == "limit_except" || token == "types")
        return (true);
    return (false);   
}

void Config::parseLocationDirective(const std::string &token, std::istringstream &iss, std::ifstream &infile, LocationConfig &locationConfig)
{
    if (validLocationDirective(token))
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
        if (checkValueNum(token, value))
        {
            if (token == "root")
                parseRoot(value, LocationCnfig)
            else if (token == "index")
                parseIndex(value, LocationConfig);
            else if (token == "autoindex")
                parseAutoindex(value, LocationConfig);
        }
        if (token == "return")
            parseRedirect(value, LocationConfig)
        else if (token == "limit_except")
            parseLimitExcept(infile, LocationConfig);
        else if (token == "types")
            parseTypes(value, LocationConfig);
    }
    else
    {
        _error = "invalid directive in location block";
        throw configException(_error, _configPath, _parsedLine);
    }
        
        
        
    //================================================================== 
        // if (!token.empty())
        // {
        //     std::string directive = token;
  
        //     if (directive == "allowed_methods")
        //     {
        //         std::vector<std::string> methods = splitBySpaces(value);
        //         for (size_t i = 0; i < methods.size(); i++)
        //             locationConfig.setAllowedMethod(methods[i]);
        //     }
        //     else if (directive == "limit_except")
        //     {
        //         locationConfig.setLimitExcept(true);
        //         std::vector<std::string> methods = splitBySpaces(value);
        //         for (size_t i = 0; i < methods.size(); i++)
        //             locationConfig.setLimitExcept(methods[i]);
        //     }
        //     else if (directive != "{" && directive != "}")
        //     {
        //         std::vector<std::string> values = splitBySpaces(value);
        //         for (size_t i = 0; i < values.size(); i++)
        //             locationConfig.setDirective(directive, values[i]);
        //     }
        // }
}


void Config::parseLocationBlock(std::ifstream &infile, LocationConfig &locationConfig)
{
    std::string line;
    int openingBrace = 0;
    int closingBrace = 0;
    
    while(std::getline(infile, line))
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
        parseLocationDirective(token, iss, infile, locationConfig);
    }
    if (openingBrace != closingBrace)
    {
        _error = "mismatch braces in location block";
        throw configException(_error, _configPath, _parsedLine);
    }
}