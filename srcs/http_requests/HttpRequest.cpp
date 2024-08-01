/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 02:18:22 by jdagoy            #+#    #+#             */
/*   Updated: 2024/08/01 01:56:53 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include "webserv.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>

HttpRequest::HttpRequest()
    : _method(""), _uri(""), _version(""), _body(""), _status(0)
{
    // parseHttpRequest();
}

HttpRequest::HttpRequest(const HttpRequest &copy)
{
    *this = copy;
}

HttpRequest &HttpRequest::operator=(const HttpRequest &copy)
{
    if (this != &copy)
    {
        _method = copy._method;
        _uri = copy._uri;
        _version = copy._version;
        _headers = copy._headers;
        _body = copy._body;
        _status = copy._status;
    }
    return (*this);
}

HttpRequest::~HttpRequest()
{
}

void HttpRequest::parseHttpRequest()
{
    while (true)
    {
        std::string line = getLineAndPopFromBuffer();
        if (line.empty())
            break; //! Handle more errors; header size, bad request etc
        std::cout << "Parsing lines" << std::endl;

        // parseRequestLine()
    }
}

// void HttpRequest::parseRequestLine()
// {
//     std::stringstream   
// }

// void HttpRequest::parseRequestHeader()
// {
    
// }

// void HttpRequest::parseRequestBody()
// {
    
// }

void    HttpRequest::requestToBuffer(int client_socket)
{
    char    buffer[1024];
    int     bytes_read;

    while ((bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0)
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