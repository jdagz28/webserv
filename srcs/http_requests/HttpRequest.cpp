/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 02:18:22 by jdagoy            #+#    #+#             */
/*   Updated: 2024/08/01 04:39:53 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include "webserv.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>

HttpRequest::HttpRequest(int client_socket)
    : _request(), _body(""), _status(0), _error(0), _client_socket(client_socket)
{
    requestToBuffer();
    printBuffer();
    parseHttpRequest();

    std::cout << "===== REQUEST LINE =====" << std::endl;
    std::cout << "Method:" << _request.getMethod() << std::endl;
    std::cout << "URI: " << _request.getUri() << std::endl;
    std::cout << "Version: " << _request.getVersion() << std::endl;
    std::cout << "========================\n" << std::endl;
}

HttpRequest::~HttpRequest()
{
}

void HttpRequest::parseHttpRequest()
{
    while (!_buffer.empty())
    {
        std::string line = getLineAndPopFromBuffer();
        // if (line.empty())
        //     break; //! Handle more errors; header size, bad request etc
        std::cout << "===== PARSING LINES =====\n" << std::endl;

        parseRequestLine(line);
        break ;
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
    
    // parse method
    if (_request.getMethod().empty())
    {
        std::string method = extract_token(line, pos, ' '); 
        if (method.empty())
        {
            std::cerr << "Error: failed to parse method." << std::endl;
            return ;
        }
        _request.setMethod(method);
    }

    // parse request-target
    if (_request.getUri().empty())
    {
        std::string uri = extract_token(line, pos, ' ');
        if (uri.empty())
        {
            std::cerr << "Error: failed to parse uri" << std::endl;
            return ;
        }
        _request.setUri(uri);
    }

    // parse http-version
    if (_request.getVersion().empty())
    {
        std::string version = extract_token(line, pos, ' ');
        if (version.empty())
        {
            std::cerr << "Error: failed to parse version" << std::endl;
            return ;
        }
        _request.setVersion(version);
    }
}

// void HttpRequest::parseRequestHeader()
// {
    
// }

// void HttpRequest::parseRequestBody()
// {
    
// }

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
    if (_buffer.empty())
        return ("");
    std::vector<unsigned char>::const_iterator it;
    std::string result = get_line(_buffer, _buffer.begin(), &it);
    if (result.empty())
        return ("");
    _buffer.erase(_buffer.begin(), _buffer.begin() + result.size());
    return (result);
}

void    HttpRequest::printBuffer() const
{
    std::vector<unsigned char>::const_iterator it;
    for (it = _buffer.begin(); it != _buffer.end(); it++)
        std::cout << *it;
}

std::string HttpRequest::get_line(const std::vector<unsigned char> &buffer,
                                    std::vector<unsigned char>::const_iterator start,
                                    std::vector<unsigned char>::const_iterator* end)
{
    *end = std::find(start, buffer.end(), '\n');
    if (*end == buffer.end())
        return ("");
    return (std::string(start, *end));
}

void    HttpRequest::setClientSocket(int client_socket)
{
    _client_socket = client_socket;
}