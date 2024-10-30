/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilities.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 22:29:37 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/22 10:10:54 by jdagoy           ###   ########.fr       */
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


bool isSpace(unsigned char ch)
{
    return (std::isspace(ch));
}
bool isNotSpace(unsigned char ch)
{
    return (!std::isspace(ch));
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

bool endsWith(const std::string &str, const std::string &suffix)
{
    size_t str_len = str.length();
    size_t suffix_len = suffix.length();
    if (suffix_len > str_len)
        return (false);
    return (str.compare(str_len - suffix_len, suffix_len, suffix) == 0);
}

std::string getExtension(const std::string &path)
{
    size_t dot_pos = path.find_last_of('.');
    if (dot_pos == std::string::npos)
        return (std::string());
    return (path.substr(dot_pos + 1));
}

std::string toString(int num)
{
    std::ostringstream oss;
    oss << num;
    return (oss.str());
}

bool validProtocol(const std::string &str)
{
    if (str.substr(0, 7) == "http://")
        return (true);
    else if (str.substr(0, 87) == "https://")
        return (true);
    return (false);
}

int strToInt(const std::string &str)
{
    std::stringstream ss(str);
    int num;

    ss >> num;
    if (ss.fail())
        return (-1);
    return (num);
}

std::string toLower(const std::string &str)
{
    if (str.empty())
        return (std::string());
    std::string lower;
    lower.resize(str.size());
    for (size_t i = 0; i < str.size(); i++)
        lower[i] = std::tolower(static_cast<unsigned char>(str[i]));
    return (lower);
}
