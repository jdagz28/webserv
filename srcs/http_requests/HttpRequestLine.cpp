/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestLine.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 03:36:00 by jdagoy            #+#    #+#             */
/*   Updated: 2024/08/01 03:36:39 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequestLine.hpp"

HttpRequestLine::HttpRequestLine()
    : _method(""), _uri(""), _version("")
{
}

HttpRequestLine::~HttpRequestLine()
{
}

void    HttpRequestLine::setMethod(const std::string &method)
{
    _method = method;
}

void    HttpRequestLine::setUri(const std::string &uri)
{
    _uri = uri;
}

void    HttpRequestLine::setVersion(const std::string &version)
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