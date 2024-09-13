/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 02:18:22 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/13 09:49:01 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include "webserv.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <ctime>
#include <sys/stat.h>
#include <fstream>

HttpRequest::HttpRequest(int client_socket)
    : _request(),  _headersN(0), _status(OK), _errorMsg(""), _client_socket(client_socket)
{
    requestToBuffer();
    // printBuffer();
    if (_errorMsg.empty())
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
                if (_status != OK)
                    return ;
                currentStep = REQUEST_HEADER;
                continue;
            case REQUEST_HEADER:
                if (line.empty())
                {
                    currentStep = REQUEST_BODY;
                    break ; 
                }
                parseRequestHeaders(line);
                _headersN++;
                if (_status != OK)
                    return ;
                continue;
            // case REQUEST_BODY:
            //     parseRequestBody(line);
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
        setStatusCode(BAD_REQUEST);
        return ;
    }

    std::string fieldName = line.substr(0, colonPos);
    std::string fieldValue = line.substr(colonPos + 1);
    
    for (size_t i = 0; i < fieldName.size(); i++)
        fieldName[i] = std::tolower(fieldName[i]);
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


void    HttpRequest::requestToBuffer()
{
    char    buffer[1024];
    int     bytes_read;

    bytes_read = recv(_client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read == 0)
    {
        _errorMsg = "Error: Client closed the connection";
        return ;
    }
    if (bytes_read < 0)
    {
       _errorMsg = "Error: receiving request to buffer";
    }
    
    buffer[bytes_read] = '\0';
    for (int i = 0; i < bytes_read; i++)
        _buffer.push_back(buffer[i]);
}

std::string    HttpRequest::getLineAndPopFromBuffer()
{
    std::string line;
    std::string::size_type pos = 0;
    
    if (_buffer.empty())
        return (std::string());
    std::string bufferStr(_buffer.begin(), _buffer.end());
    std::string::size_type newlinePos = bufferStr.find('\n');
    if (newlinePos != std::string::npos)
    {
        line = bufferStr.substr(0, newlinePos);
        if (!line.empty() && line[line.size() - 1] == '\r')
            line = line.substr(0, line.size() - 1);
        pos = newlinePos + 1;
        _buffer.erase(_buffer.begin(), _buffer.begin() + pos);
    }   
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

const std::map<std::string, std::vector<std::string> >& HttpRequest::getHeaders() const
{
    return (_headers);
}

bool HttpRequest::isConnectionClosed() const
{
    std::map<std::string, std::vector<std::string> >::const_iterator header;
    header = _headers.find("Connection");
    
    if (header == _headers.end())
        return (false);
    std::vector<std::string>::const_iterator value;
    for (value = header->second.begin(); value != header->second.end(); value++)
    {
        if (*value == "close")
            return (true);
    }
    return (false);
}

std::string HttpRequest::getHost() const
{
    std::map<std::string, std::vector<std::string> >::const_iterator header;
    header =_headers.find("host");
    if (header != _headers.end())
        return (header->second[0]);
    return (std::string());
}

void HttpRequest::setStatusCode(StatusCode status)
{
    _status = status;
}

StatusCode HttpRequest::getStatusCode() const
{
    return (_status);
}

const std::string &HttpRequest::getErrorMsg() const
{
    return (_errorMsg);
}

const std::string HttpRequest::getHeader(const std::string &field) const
{
    std::map<std::string, std::vector<std::string> > ::const_iterator header;
    header = _headers.find(field);
    if (header != _headers.end())
        return (header->second[0]);
    return (std::string());
}


void HttpRequest::parseRequestBody(const std::string &line)
{
    std::string type = getHeader("Content-Type");
    
    if (type.empty())
    {
        setStatusCode(UNSUPPORTED_MEDIA_TYPE);
        return ;
    }
    if (type == "application/x-www-form-urlencoded")
    {
        parseFormData(line);
        if (_status != OK)
            return ;
    }
    else if (type == "image/jpeg" || type == "image/gif" || type == "image/png" || type == "image/bmp")
    {
        processImageUpload(line, type);
        if (_status != OK)
            return ;
    }
    else
    {
        setStatusCode(UNSUPPORTED_MEDIA_TYPE);
        return ;
    }
}

bool HttpRequest::isSupportedMediaPOST()
{
    std::string type = getHeader("Content-Type");
    if (type.empty())
    {
        setStatusCode(UNSUPPORTED_MEDIA_TYPE);
        return (false);
    }
    if (type == "application/x-www-form-urlencoded" || type == "multipart/form-data")
        return (true);
    if (type == "image/jpeg" || type == "image/gif" || type == "image/png" || type == "image/bmp")
        return (true);
    setStatusCode(UNSUPPORTED_MEDIA_TYPE);
    return (false);
}

void HttpRequest::parseFormData(const std::string &line)
{
    std::stringstream ss(line);
    std::string pair;

    while (std::getline(ss, pair, '&'))
    {
        size_t equalPos = pair.find('=');
        if (equalPos == std::string::npos)
        {
            setStatusCode(BAD_REQUEST);
            return ;
        }
        std::string key = pair.substr(0, equalPos);
        std::string value = pair.substr(equalPos + 1);
        _formData[key] = value;
    }
}

std::string HttpRequest::generateFilename(const std::string &type)
{
    std::time_t now = std::time(NULL);
    std::tm *tm = std::localtime(&now);

    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", tm);
    std::string timestamp(buffer);

    std::string extension = type.substr(6);
    std::string filename = "image_" + timestamp + "." + extension;

    return (filename);
}

void HttpRequest::processImageUpload(const std::string &line, const std::string &type)
{
    std::srand(static_cast<unsigned>(std::time(0)));
    std::string filename = generateFilename(type);
    if (filename.empty())
    {
        setStatusCode(INTERNAL_SERVER_ERROR);
        return ;
    }
    
    std::string directory = "upload/";
    struct stat st;
    if (stat(directory.c_str(), &st) == -1 || !S_ISDIR(st.st_mode))
    {
        setStatusCode(INTERNAL_SERVER_ERROR);
        return ;
    }

    std::string filepath = directory + filename;
    std::ofstream filestream(filepath.c_str(), std::ios::binary);
    if (!filestream)
    {
        setStatusCode(INTERNAL_SERVER_ERROR);
        return ;
    }
    
    filestream.write(line.c_str(), line.size());
    filestream.close();
    setStatusCode(CREATED);
}