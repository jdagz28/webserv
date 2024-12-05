/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generateResponse.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 01:11:06 by jdagoy            #+#    #+#             */
/*   Updated: 2024/11/07 11:49:24 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "webserv.hpp"
#include "ServerConfig.hpp"
#include "HttpRequestLine.hpp"
#include "LocationConfig.hpp"
#include <string>
#include <ctime>
#include <sstream>

void HttpResponse::generateHttpResponse()
{
    if (_body.empty() && _status != OK)
        getErrorPage();
    
    _headers["Server"] = _serverName;
    _headers["Date"] = getHttpDateGMT();
    if (!_body.empty())
        _headers["Content-Length"] = toString(_body.size());
    addKeepAliveHeader();
    addAllowHeader();
    
    std::string statusLine = generateStatusLine();
    std::string headerLines = generateHeaderLines();
    std::string empty = CRLF;

    if (!_responseMsg.empty())
        _responseMsg.clear();
    _responseMsg.insert(_responseMsg.end(), statusLine.begin(), statusLine.end());
    _responseMsg.insert(_responseMsg.end(), headerLines.begin(), headerLines.end());
    _responseMsg.insert(_responseMsg.end(), empty.begin(), empty.end());
    _responseMsg.insert(_responseMsg.end(), _body.begin(), _body.end());
}

std::string HttpResponse::getHttpDateGMT()
{
    std::time_t now = std::time(NULL);
    std::tm *gmt_tm = std::gmtime(&now);
    
    const int kDateBufSize = 1024;
    char date[kDateBufSize];
    std::strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S GMT", gmt_tm);
    
    return (std::string(date));
}

bool HttpResponse::isKeepAlive() const
{
    if (_request.isConnectionClosed())
        return (false);
    if (_config.getKeepAliveTimeout() == 0)
        return (false);
    return (true);
}


void HttpResponse::addKeepAliveHeader()
{
    if (isKeepAlive())
    {
        _headers["Connection"] = "keep-alive";
        std::ostringstream keepalive;
        keepalive << "time=" << _config.getKeepAliveTimeout();
        keepalive << ", max=100"; //! MAX connections
        _headers["Keep-Alive"] = keepalive.str();
    }
    else
        _headers["Connection"] = "close";
}

void HttpResponse::addAllowHeader()
{
    if (static_cast<int>(getStatusCode()) >= 400 ) 
        return ;
    
    std::string allowed_method;
    if (!_locationConfig.getPath().empty())
    {
        std::vector<std::string> allowedMethods = _locationConfig.getAllowedMethods();
        for (size_t i = 0; i < allowedMethods.size(); i++)
        {
            if (i != 0)
                allowed_method += ", ";
            allowed_method += allowedMethods[i];
        }
        _headers["Allow"] = allowed_method;
    }
}

std::string HttpResponse::generateStatusLine()
{
    std::ostringstream status_line;
    
    status_line << "HTTP/1.1" << " ";
    status_line << toString(static_cast<int>(_status));
    status_line << " ";
    status_line << getStatusReason(_status);
    status_line << CRLF;
    return (status_line.str());   
}

std::string HttpResponse::generateHeaderLines()
{
    std::map<std::string, std::string>::const_iterator header;
    std::ostringstream response_headers;

    for (header = _headers.begin(); header != _headers.end(); header++)
    {
        std::string field_name = header->first;
        std::string field_value = header->second;

        response_headers << field_name << ": " << field_value << CRLF;
    }
    return (response_headers.str());
}

void HttpResponse::addContentTypeHeader(const std::string &type)
{
    if (type.empty())
        _headers["Content-Type"] = "text/html";
    else
        _headers["Content-Type"] = getMimeType(type);
}

std::string HttpResponse::getHeader(const std::string &header)
{
    std::map<std::string, std::string>::iterator it;
    for (it = _headers.begin(); it != _headers.end(); it++)
    {
        if (it->first == header)
            return (it->second);
    }
    return (std::string());
}

std::string HttpResponse::getStatusLine()
{
    return (generateStatusLine());
}