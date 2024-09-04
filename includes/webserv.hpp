/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 22:28:27 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/03 03:54:13 by jdagoy           ###   ########.fr       */
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

#define CRLF "\r\n"

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

enum StatusCode
{
    OK                          = 200,
    CREATED                     = 201, // POST
    NO_CONTENT                  = 204, // DELETE
    
    MOVED_PERMANENTLY           = 301,

    BAD_REQUEST                 = 400,
    UNAUTHORIZED                = 401,
    FORBIDDEN                   = 403,
    NOT_FOUND                   = 404,
    METHOD_NOT_ALLOWED          = 405,
    REQUEST_TIMEOUT             = 408,
    PAYLOAD_TOO_LARGE           = 413,
    URI_TOO_LONG                = 414,
    UNSUPPORTED_MEDIA_TYPE      = 415,
    REQUEST_HEADER_TOO_LARGE    = 431,

    INTERNAL_SERVER_ERROR       = 500,
    NOT_IMPLEMENTED             = 501,
    BAD_GATEWAY                 = 502,
    SERVICE_UNAVAILABLE         = 503,
    
    INIT                        = 0
};

// UTILITIES
void    trimWhitespaces(std::string &str);
std::vector<std::string>    splitBySpaces(const std::string &str);
bool isDirectory(const std::string &path);
bool fileExists(const std::string &path);
bool endsWith(const std::string &str, const std::string &suffix);
std::string getExtension(const std::string &path);
std::string toString(int num);

// MIMETYPES.CPP
std::string getMimeType(const std::string &extension);

//STATUSCODES.CPP
std::string getStatusReason(StatusCode code);








#endif