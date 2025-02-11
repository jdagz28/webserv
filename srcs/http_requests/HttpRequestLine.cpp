/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestLine.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 03:36:00 by jdagoy            #+#    #+#             */
/*   Updated: 2025/02/11 12:27:17 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequestLine.hpp"

HttpRequestLine::HttpRequestLine()
    : _method(""), _uri(""), _version(""), _error(0), _errormsg("")
{}

HttpRequestLine::HttpRequestLine(const HttpRequestLine &copy)
    : _method(copy._method), 
		_uri(copy._uri), 
		_version(copy._version), 
		_error(copy._error), 
		_errormsg(copy._errormsg)
{}

HttpRequestLine &HttpRequestLine::operator=(const HttpRequestLine &copy)
{
    if (this != &copy)
    {
        _method = copy._method;
        _uri = copy._uri;
        _version = copy._version;
        _error = copy._error;
        _errormsg = copy._errormsg;
    }
    return (*this);
}

HttpRequestLine::~HttpRequestLine()
{}

void	HttpRequestLine::setMethod(const std::string &method)
{
    _method = method;
}

void	HttpRequestLine::setUri(const std::string &uri)
{
    _uri = uri;
}

void	HttpRequestLine::setVersion(const std::string &version)
{
    _version = version;
}

const std::string	&HttpRequestLine::getMethod() const
{
    return (_method);
}

const std::string	&HttpRequestLine::getUri() const
{
    return (_uri);
}

const std::string	&HttpRequestLine::getVersion() const
{
    return (_version);
}

const std::string	&HttpRequestLine::getErrormsg() const
{
    return (_errormsg);
}

int	HttpRequestLine::getError() const
{
    return (_error);
}
