/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseHeader.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 12:42:30 by jdagoy            #+#    #+#             */
/*   Updated: 2025/03/06 10:42:53 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include "webserv.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>

std::string	HttpRequest::extract_token(const std::string &line, size_t &pos, char del)
{
    size_t  end = line.find(del, pos);
    if (end == std::string::npos)
        end = line.length();
    std::string token = line.substr(pos, end - pos);
    pos = end + 1;
    return (token);
}

void	HttpRequest::parseRequestLine(const std::string &line)
{
    size_t  pos = 0;
    
    if (_request.getMethod().empty())
    {
        std::string method = extract_token(line, pos, ' '); 
        if (method.empty())
        {
            setStatusCode(BAD_REQUEST);
            return ;
        }
        trimWhitespaces(method);
        _request.setMethod(method);
    }

    if (_request.getUri().empty())
    {
        std::string uri = extract_token(line, pos, ' ');
        if (uri.empty())
        {
            setStatusCode(BAD_REQUEST);
            return ;
        }
        if (uri.length() > MAX_URI_LENGTH)
        {
            setStatusCode(URI_TOO_LONG);
            return ;
        }
        trimWhitespaces(uri);
        _request.setUri(uri);
    }

    if (_request.getVersion().empty())
    {
        std::string version = extract_token(line, pos, ' ');
        if (version.empty())
        {
            setStatusCode(BAD_REQUEST);
            return ;
        }
        trimWhitespaces(version);
        _request.setVersion(version);
    }
}

bool	HttpRequest::isValidFieldName(const std::string &line)
{
    if (line.empty())
        return (false);
    for (size_t i = 0; i < line.size(); i++)
        if (!std::isalnum(line[i]) && line[i] != '-')
            return (false);
    return (true);
}

bool	HttpRequest::isValidFieldValue(const std::string &line)
{
    if (line.empty())
        return (false);
    for (size_t i = 0; i < line.size(); i++)
    {
        if (!std::isprint(line[i]) && !std::isspace(line[i]) )
            return (false);
    }
    return (true);
}

void	HttpRequest::parseRequestHeaders(const std::string &line)
{
    size_t  colonPos = line.find (':');
    if (colonPos == std::string::npos)
    {
        setStatusCode(BAD_REQUEST);
        return ;
    }

    std::string fieldName = line.substr(0, colonPos);
    std::string fieldValue = line.substr(colonPos + 1);
    
    fieldName = toLower(fieldName);
    if (!isValidFieldName(fieldName) || !isValidFieldValue(fieldValue))
    {
        setStatusCode(BAD_REQUEST);
        return ;
    }    
    trimWhitespaces(fieldValue);
    if (fieldValue.length() > MAX_HEADER_LENGTH)
    {
        setStatusCode(REQUEST_HEADER_TOO_LARGE);
        return ;
    }
    
    std::stringstream ss(fieldValue);
    std::string value;
    std::vector<std::string> values;
    while (std::getline(ss, value, ','))
    {
        trimWhitespaces(value);
        values.push_back(value);
    }
    _headers[fieldName] = values;
}