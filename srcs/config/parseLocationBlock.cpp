/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseLocationBlock.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/04 14:50:02 by jdagoy            #+#    #+#             */
/*   Updated: 2025/02/11 10:44:38 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "webserv.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>


void	Config::checkAllowedMethod(const std::string &value, LocationConfig &locationConfig)
{
    std::vector<std::string> allowed = splitBySpaces(value);
    for (size_t i = 0; i < allowed.size(); i++)
    {
        if (allowed[i] != "GET" && allowed[i] != "POST" && allowed[i] != "DELETE")
        {
            _error = std::string("invalid value in ") + GREEN + "\"limit_except\"" + RESET + " directive";
            throw configException(_error, _configPath, _parsedLine);
        }
        locationConfig.setAllowedMethod(allowed[i]);
    }
}

void	Config::parseTypes(const std::string &value, std::ifstream &infile, LocationConfig &locationConfig)
{
    if (!value.empty())
    {
        _error = std::string("invalid value in ") + GREEN + "\"types\"" + RESET + " directive";
        throw configException(_error, _configPath, _parsedLine);
    }
    
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
        if (openingBrace == 1 && token == "text/css")
        {
            std::string value;
            std::getline(iss, value);
            trimWhitespaces(value);
            if (value != "css;")
            {
                _error = std::string("invalid value in ") + GREEN + "\"text/css\"" + RESET + " directive";
                throw configException(_error, _configPath, _parsedLine);
            }
            locationConfig.setDirective("types", token);
        }
    }
    if (openingBrace != closingBrace)
    {
        _error = "mismatch braces in one of the directives in location block";
        throw configException(_error, _configPath, _parsedLine);
    }
}

void	Config::parseLimitExcept(const std::string &value, std::ifstream &infile, LocationConfig &locationConfig)
{
    checkAllowedMethod(value, locationConfig);
    locationConfig.setDirective("limit_except", "true");

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
        if (openingBrace == 1 && token == "deny;")
        {
            std::string value;
            std::getline(iss, value);
            trimWhitespaces(value);
            if (value != "all")
            {
                _error = std::string("invalid value in ") + GREEN + "\"deny\"" + RESET + " directive";
                throw configException(_error, _configPath, _parsedLine);
            }
        }
    }
    if (openingBrace != closingBrace)
    {
        _error = "mismatch braces in one of the directives in location block";
        throw configException(_error, _configPath, _parsedLine);
    }
}

void	Config::parseRedirect(const std::string &value, LocationConfig &locationConfig)
{
    std::vector<std::string> values = splitBySpaces(value);
    if (values.size() != 2)
    {
        _error = std::string("invalid number of arguments in ") + GREEN + "\"return\"" + RESET + " directive";
        throw configException(_error, _configPath, _parsedLine);
    }
    if (values[0] != "301" && values[0] != "302" && values[0] != "303" && values[0] != "307" && values[0] != "308")
    {
        _error = std::string("invalid value in ") + GREEN + "\"return\"" + RESET + " directive";
        throw configException(_error, _configPath, _parsedLine);
    }
    for (size_t i = 0; i < values[1].size(); i++)
    {
        if (!std::isalnum(values[1][i]) && values[1][i] != '/' && values[1][i] != '.' && values[1][i] != ':' && values[1][i] != '_')
        {
            _error = std::string("invalid value in ") + GREEN + "\"return\"" + RESET + " directive";
            throw configException(_error, _configPath, _parsedLine);
        }
    }
    locationConfig.setDirective("return", value);
}

void	Config::parseAutoindex(const std::string &value, LocationConfig &locationConfig)
{
    if (value != "on" && value != "off")
    {
        _error = std::string("invalid value in ") + GREEN + "\"autoindex\"" + RESET + " directive";
        throw configException(_error, _configPath, _parsedLine);
    }
    locationConfig.setDirective("autoindex", value);
}

void	Config::parseIndex(const std::string &value, LocationConfig &locationConfig)
{
    for (size_t i = 0; i < value.size(); i++)
    {
        if (!std::isalnum(value[i]) && value[i] != '.' && value[i] != '_' && value[i] != '/') 
        {
            _error = std::string("invalid value in ") + GREEN + "\"index\"" + RESET + " directive";
            throw configException(_error, _configPath, _parsedLine);
        }
    }
    if (value.substr(value.size() - 5) != ".html")
    {
        _error = std::string("invalid value in ") + GREEN + "\"index\"" + RESET + " directive";
        throw configException(_error, _configPath, _parsedLine);
    }
    locationConfig.setDirective("index", value);
}


void	Config::parseRoot(const std::string &value, LocationConfig &locationConfig)
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

void	Config::parseClientBodySize(std::string &value, LocationConfig &locationConfig)
{
    if (value[value.length() - 1] != 'M' && value[value.length() - 1] != 'm')
    {
        _error = std::string("invalid value in ") + GREEN + "\"" + "client_max_body_size" + "\"" + RESET;
        throw configException(_error, _configPath, _parsedLine);
    }
    value = value.substr(0, value.length() - 1);
    int converted = strToInt(value);
    if (converted < 0 || converted > 10)
    {
        _error = std::string("invalid value in ") + GREEN + "\"" + "client_max_body_size" + "\"" + RESET;
        throw configException(_error, _configPath, _parsedLine);
    }
    locationConfig.setDirective("client_max_body_size", value);
}

void	Config::parseCGIMode(const std::string &value, LocationConfig &locationConfig)
{
    if (value != "on" && value != "off")
    {
        _error = std::string("invalid value in ") + GREEN + "\"cgi_mode\"" + RESET + " directive";
        throw configException(_error, _configPath, _parsedLine);
    }
    locationConfig.setDirective("cgi_mode", value);
}

void	Config::parseCGIExtensions(const std::string &value, LocationConfig &locationConfig)
{  
    std::vector<std::string> extensions = splitBySpaces(value);
    
    for (size_t i = 0; i < extensions.size(); i++)
    {
        if (extensions[i][0] != '.')
        {
            _error = std::string("invalid value in ") + GREEN + "\"cgi_extension\"" + RESET + " directive";
            throw configException(_error, _configPath, _parsedLine);
        }
        locationConfig.setCGIExtension(extensions[i]);
    }
    locationConfig.setDirective("cgi_extension", value);
}

void	Config::checkValueNum(const std::string &token, const std::string &value)
{
    if (token == "return" || token == "limit_except" || token == "types" || token == "client_max_body_size" || token == "cgi_extension")
        return ;
    std::vector<std::string> values = splitBySpaces(value);
    if (value.empty() || values.size() != 1)
    {
        _error = std::string("invalid number of arguments in ") + GREEN + "\"" + token + "\"" + RESET + " directive";
        throw configException(_error, _configPath, _parsedLine);
    }
}

bool	Config::validLocationDirective(const std::string &token)
{
    if (token == "root" || token == "index" || token == "autoindex" || token == "return" || token == "limit_except" 
        || token == "types" || token == "client_max_body_size" || token == "cgi_mode" || token == "cgi_extension")
        return (true);
    return (false);   
}

void	Config::parseLocationDirective(const std::string &token, std::istringstream &iss, std::ifstream &infile, LocationConfig &locationConfig)
{
    if (validLocationDirective(token))
    {
        std::string value;
        std::getline(iss, value);
        trimWhitespaces(value);
        if (token == "limit_except")
            parseLimitExcept(value,  infile, locationConfig);
        else if (token == "types")
            parseTypes(value, infile, locationConfig);
        else
        {       
            if (value[value.length() - 1]  != ';')
            {
                _error = std::string("missing semicolon at the end of directive ") + GREEN + "\"" + token + "\"" + RESET;
                throw configException(_error, _configPath, _parsedLine);
            }
            value = value.substr(0, value.length() - 1);
            checkValueNum(token, value);
            if (token == "root")
                parseRoot(value, locationConfig);
            else if (token == "index")
                parseIndex(value, locationConfig);
            else if (token == "autoindex")
                parseAutoindex(value, locationConfig);
            else if (token == "return")
                parseRedirect(value, locationConfig);
            else if (token == "client_max_body_size")
                parseClientBodySize(value, locationConfig);
            else if (token == "cgi_mode")
                parseCGIMode(value, locationConfig);
            else if (token == "cgi_extension")
                parseCGIExtensions(value, locationConfig);
        }
    }
    else
    {
        _error = "invalid directive in location block";
        throw configException(_error, _configPath, _parsedLine);
    }
}


void	Config::parseLocationBlock(std::ifstream &infile, LocationConfig &locationConfig)
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