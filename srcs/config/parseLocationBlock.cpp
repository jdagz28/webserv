/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseLocationBlock.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/04 14:50:02 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/04 15:29:10 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "webserv.hpp"
#include <iostream>
#include <fstream>
#include <sstream>



void Config::parseRoot(const std::string &value, LocationConfig &locationConfig)
{

}

void Config::checkValueNum(const std::string &token, const std::string &value)
{
    if (token == "return" || token == "limit_except")
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
    if (token == "root" || token == "index" || token == "autoindex" || token == "return" || token == "limit_except")
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