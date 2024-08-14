/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 22:28:27 by jdagoy            #+#    #+#             */
/*   Updated: 2024/08/14 06:27:23 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <string>
#include <vector>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

enum HtmlRequestParseStep
{
    REQUEST_LINE,
    REQUEST_HEADER,
    REQUEST_BODY
};

enum Methods
{
    GET,
    POST,
    DELETE,
    ErrorMethod
};

// UTILITIES
void    trimWhitespaces(std::string &str);
std::vector<std::string>    splitBySpaces(const std::string &str);
bool isDirectory(const std::string &path);
bool fileExists(const std::string &path);
bool endsWith(const std::string &str, const std::string &suffix);









#endif