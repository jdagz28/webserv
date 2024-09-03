/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 02:18:22 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/02 07:00:38 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include "webserv.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>

HttpRequest::HttpRequest(int client_socket)
    : _request(),  _headersN(0), _status(0), _error(0), _errorMsg(""), _client_socket(client_socket)
{
    requestToBuffer();
    // printBuffer();
    parseHttpRequest();
}

HttpRequest::~HttpRequest()
{
}

void HttpRequest::parseHttpRequest()
{
    HtmlRequestParseStep currentStep = REQUEST_LINE;
    
    while (!_buffer.empty())
    {
        std::string line = getLineAndPopFromBuffer();
        // std::cout << line << std::endl;
        switch (currentStep)
        {
            case REQUEST_LINE:
                parseRequestLine(line);
                if (_request.getError() != 0)
                {
                    std::cerr << _request.getErrormsg() << std::endl;
                    _error = 1;
                    return ;
                }
                currentStep = REQUEST_HEADER;
                continue;
            case REQUEST_HEADER:
                if (line.empty())
                    break ; 
                parseRequestHeaders(line);
                _headersN++;
                continue;
            default:
                break;
        }
    }
}

std::string    HttpRequest::extract_token(const std::string &line, size_t &pos, char del)
{
    size_t  end = line.find(del, pos);
    if (end == std::string::npos)
        end = line.length();
    std::string token = line.substr(pos, end - pos);
    pos = end + 1;
    return (token);
}



void    HttpRequest::parseRequestLine(const std::string &line)
{
    size_t  pos = 0;
    
    if (_request.getMethod().empty())
    {
        std::string method = extract_token(line, pos, ' '); 
        if (method.empty())
        {
            _error = 1;
            std::cerr << "Error: failed to parse method." << std::endl;
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
            _error = 1;
            std::cerr << "Error: failed to parse uri" << std::endl;
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
            _error = 1;
            std::cerr << "Error: failed to parse version" << std::endl;
            return ;
        }
        trimWhitespaces(version);
        _request.setVersion(version);
    }
}

bool    HttpRequest::isValidFieldName(const std::string &line)
{
    if (line.empty())
        return (false);
    for (size_t i = 0; i < line.size(); i++)
        if (!std::isalnum(line[i]) && line[i] != '-')
            return (false);
    return (true);
}

bool    HttpRequest::isValidFieldValue(const std::string &line)
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

void HttpRequest::parseRequestHeaders(const std::string &line)
{
    size_t  colonPos = line.find (':');
    if (colonPos == std::string::npos)
    {
        _error = 1;
        std::cerr <<"Error: Bad request, missing colon in field line" << std::endl;
        return ;
    }

    std::string fieldName = line.substr(0, colonPos);
    std::string fieldValue = line.substr(colonPos + 1);
    
    for (size_t i = 0; i < fieldName.size(); i++)
        fieldName[i] = std::tolower(fieldName[i]);
    if (!isValidFieldName(fieldName) || !isValidFieldValue(fieldValue))
    {
        _error = 1;
        std::cerr << "Error: Invalid field name or value" << std::endl;
        return ;
    }    
    trimWhitespaces(fieldValue);
    std::stringstream ss(fieldValue);
    std::string value;
    std::vector<std::string> values;
    while (std::getline(ss, value, ','))
    {
        trimWhitespaces(value);
        values.push_back(value);
    }
    _headers.push_back(std::make_pair(fieldName, values));
}


void    HttpRequest::requestToBuffer()
{
    char    buffer[1024];
    int     bytes_read;

    while ((bytes_read = recv(_client_socket, buffer, sizeof(buffer) - 1, 0)) > 0)
    {
        buffer[bytes_read] = '\0';
        for (int i = 0; i < bytes_read; i++)
            _buffer.push_back(buffer[i]);
    }
}

std::string    HttpRequest::getLineAndPopFromBuffer()
{
    std::string line;
    std::string::size_type pos = 0;
    
    if (_buffer.empty())
        return ("");
    std::string bufferStr(_buffer.begin(), _buffer.end());
    std::string::size_type newlinePos = bufferStr.find('\n');
    if (newlinePos != std::string::npos)
    {
        line = bufferStr.substr(0, newlinePos);
        if (!line.empty() && line[line.size() - 1] == '\r')
            line = line.substr(0, line.size() - 1);
        pos = newlinePos + 1;
    }
    _buffer.erase(_buffer.begin(), _buffer.begin() + pos);
    return (line);
}

void    HttpRequest::printBuffer() const
{
    std::vector<unsigned char>::const_iterator it;
    for (it = _buffer.begin(); it != _buffer.end(); it++)
        std::cout << *it;
}

void    HttpRequest::setClientSocket(int client_socket)
{
    _client_socket = client_socket;
}

const HttpRequestLine& HttpRequest::getRequestLine() const
{
    return (_request);
}

const std::vector<std::pair<std::string, std::vector<std::string> > >& HttpRequest::getHeaders() const
{
    return (_headers);
}

bool HttpRequest::isConnectionClosed() const
{
    std::vector<std::pair<std::string, std::vector<std::string> > >::const_iterator header;

    for (header = _headers.begin(); header != _headers.end(); header++)
    {
        if (header->first == "Connection")
        {
            std::vector<std::string>::const_iterator value;
            for (value = header->second.begin(); value != header->second.end(); value++)
            {
                if (*value == "close")
                    return (true);
            }
        }
    }
    return (false);
}