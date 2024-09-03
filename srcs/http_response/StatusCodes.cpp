/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StatusCodes.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 01:04:16 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/03 03:24:42 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <map>
#include <string>
#include "webserv.hpp"


std::string getStatusReason(StatusCode code) 
{
    switch (code) 
    {
        case OK:
            return "OK";
        case CREATED:
            return "Created";
        case NO_CONTENT:
            return "No Content";
        case MOVED_PERMANENTLY:
            return "Moved Permanently";
        case BAD_REQUEST:
            return "Bad Request";
        case UNAUTHORIZED:
            return "Unauthorized";
        case FORBIDDEN:
            return "Forbidden";
        case NOT_FOUND:
            return "Not Found";
        case METHOD_NOT_ALLOWED:
            return "Method Not Allowed";
        case REQUEST_TIMEOUT:
            return "Request Timeout";
        case PAYLOAD_TOO_LARGE:
            return "Payload Too Large";
        case URI_TOO_LONG:
            return "URI Too Long";
        case UNSUPPORTED_MEDIA_TYPE:
            return "Unsupported Media Type";
        case REQUEST_HEADER_TOO_LARGE:
            return "Request Header Fields Too Large";
        case INTERNAL_SERVER_ERROR:
            return "Internal Server Error";
        case NOT_IMPLEMENTED:
            return "Not Implemented";
        case BAD_GATEWAY:
            return "Bad Gateway";
        case SERVICE_UNAVAILABLE:
            return "Service Unavailable";
        case INIT:
            return "Status Init";
 
        default:
            return "Unknown Status Code";
    }
}