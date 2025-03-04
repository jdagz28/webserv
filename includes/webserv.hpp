/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 22:28:27 by jdagoy            #+#    #+#             */
/*   Updated: 2025/01/14 13:34:15 by jdagoy          ###   ########.fr       */
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
#include <csignal>

#define CRLF "\r\n"
#define MAX_URI_LENGTH 1000
#define MAX_HEADER_LENGTH 1000
#define DEFAULT_SERVERNAME "webserv"
#define LOCALHOST "127.0.0.1"
#define MAX_CLIENTS 1000

const std::string RED = "\033[0;31m";
const std::string GREEN = "\033[0;32m";
const std::string YELLOW = "\033[1;33m";
const std::string BLUE = "\033[0;34m";
const std::string RESET = "\033[0m";

typedef int socketFD;
typedef int clientFD;
typedef int FD;

extern volatile sig_atomic_t g_running;

enum HtmlRequestParseStep
{
    REQUEST_INIT,
    REQUEST_LINE,
    REQUEST_HEADER,
    REQUEST_BODY,
    REQUEST_DONE
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
    TEMP_FOUND                  = 302,
    SEE_OTHER                   = 303,
    TEMP_REDIRECT               = 307,
    PERMANENT_REDIRECT          = 308,

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
bool	isDirectory(const std::string &path);
bool	fileExists(const std::string &path);
bool	endsWith(const std::string &str, const std::string &suffix);
std::string	getExtension(const std::string &path);
std::string	toString(int num);
bool	validProtocol(const std::string &str);
int	strToInt(const std::string &str);
bool	isSpace(unsigned char ch);
std::string	toLower(const std::string &str);
void	signalHandler(int signum);
std::string	cleanMessage(const std::string &message);

// MIMETYPES.CPP
std::string	getMimeType(const std::string &extension);

//STATUSCODES.CPP
std::string getStatusReason(StatusCode code);








#endif