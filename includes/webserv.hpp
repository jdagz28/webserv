/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 22:28:27 by jdagoy            #+#    #+#             */
/*   Updated: 2024/08/07 04:03:01 by jdagoy           ###   ########.fr       */
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










#endif