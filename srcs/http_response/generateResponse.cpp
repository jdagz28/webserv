/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generateResponse.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 01:11:06 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/11 01:12:24 by jdagoy           ###   ########.fr       */
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
    _headers["Server"] = _serverName;
    _headers["Date"] = getHttpDateGMT();
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
    if (getStatusCode() != METHOD_NOT_ALLOWED) 
        return ;
    
    const std::vector<ServerConfig> &serverConfigs = _config.getServerConfig();
    if (serverConfigs.empty())
    {
        setStatusCode(NOT_FOUND);
        return ;
    }
    std::vector<ServerConfig>::const_iterator server;
    
    for (server = serverConfigs.begin(); server != serverConfigs.end(); server++)
    {
        std::string path = comparePath(*server, _request.getRequestLine());
        if (path.empty())
        {
            setStatusCode(NOT_FOUND);
            return ;
        }
        std::string target_path = _request.getRequestLine().getUri();
        
        const std::vector<LocationConfig> &locationConfigs = server->getLocationConfig();
        if (locationConfigs.empty())
        {
            setStatusCode(NOT_FOUND);
            return ;
        }
        std::vector<LocationConfig>::const_iterator location;
        for (location = locationConfigs.begin(); location != locationConfigs.end(); location++)
        {
            std::string config_location = location->getPath();
            if (config_location == target_path)
            {
                std::string allowed_method;
                if (location->isLimited())
                {
                    std::vector<std::string> limitExceptMethods = location->getLimitExcept();
                    for (size_t i = 0; i < limitExceptMethods.size(); i++)
                    {
                        if (i != 0)
                            allowed_method += ", ";
                        allowed_method += limitExceptMethods[i];
                    }
                    _headers["Allow"] = allowed_method;
                }
                else
                {
                    std::vector<std::string> allowedMethods = location->getAllowedMethods();
                    for (size_t i = 0; i < allowedMethods.size(); i++)
                    {
                        if (i != 0)
                            allowed_method += ", ";
                        allowed_method += allowed_method[i];
                    }
                    _headers["Allow"] = allowed_method;
                }
            }
        }
    }
}

std::string HttpResponse::generateStatusLine()
{
    std::ostringstream status_line;
    
    status_line << "HTTP/1.1" << " ";
    status_line << static_cast<int>(getStatusCode()) << " ";
    status_line << getStatusReason(getStatusCode());
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