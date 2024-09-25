/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestLine.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 03:36:00 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/25 12:49:46 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequestLine.hpp"

HttpRequestLine::HttpRequestLine()
    : _method(""), _uri(""), _version(""), _error(0), _errormsg("")
{
}

HttpRequestLine::~HttpRequestLine()
{
}

void HttpRequestLine::setMethod(const std::string &method)
{
    _method = method;
}

void HttpRequestLine::setUri(const std::string &uri)
{
    _uri = uri;
}

void HttpRequestLine::setVersion(const std::string &version)
{
    _version = version;
}

const std::string &HttpRequestLine::getMethod() const
{
    return (_method);
}

const std::string &HttpRequestLine::getUri() const
{
    return (_uri);
}

const std::string &HttpRequestLine::getVersion() const
{
    return (_version);
}

const std::string &HttpRequestLine::getErrormsg() const
{
    return (_errormsg);
}

int HttpRequestLine::getError() const
{
    return (_error);
}

void HttpRequestLine::validate()
{
    if (_method != "GET" || _method != "POST" || _method != "DELETE")
    {
        _error = 1;
        _errormsg = "Error: Invalid method.";
        return ;
    }
    //TODO: URI checks?
    if (_version != "HTTP/1.1")
    {
        _error = 1;
        _errormsg = "Error: Invalid version.";
        return ;
    }
}