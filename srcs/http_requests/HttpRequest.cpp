/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 02:18:22 by jdagoy            #+#    #+#             */
/*   Updated: 2024/07/30 23:49:04 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include "webserv.hpp"

HttpRequest::HttpRequest()
    : _method(""), _uri(""), _version(""), _body(""), _status(0)
{
    parseHttpRequest();
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

HttpRequest::pareseHttpRequest()
{
    
}