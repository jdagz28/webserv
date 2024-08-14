/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilities.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 22:29:37 by jdagoy            #+#    #+#             */
/*   Updated: 2024/08/14 05:14:53 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#include <sys/stat.h>


std::vector<std::string>    splitBySpaces(const std::string &str)
{
    std::istringstream iss(str);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token)
        tokens.push_back(token);
    return (tokens);
}


bool isNotSpace(unsigned char ch)
{
    return !std::isspace(ch);
}

void    trimWhitespaces(std::string &str)
{
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), isNotSpace));
    str.erase(std::find_if(str.rbegin(), str.rend(), isNotSpace).base(), str.end());
}

bool isDirectory(const std::string &path)
{
    struct stat sb;
    if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
        return (true);
    return (false);
}

bool fileExists(const std::string &path)
{
    struct stat sb;
    if (stat(path.c_str(), &sb) == 0 && S_ISREG(sb.st_mode))
        return (true);
    return (false);
}