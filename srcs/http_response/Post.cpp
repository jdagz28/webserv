/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Post.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 10:57:50 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/19 02:52:54 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "webserv.hpp"
#include "ServerConfig.hpp"
#include "HttpRequestLine.hpp"
#include "LocationConfig.hpp"
#include <string>

/**
 *  STATIC
 *  - check Loc if POST method is allowed
 *  - check if Form
 *  - check if supported media
 */


/**
 *  POST Media 
 *  - form data (multi-part, single form(search and redirect to Google))
 *  - image upload (jpeg, jpg, png, gif)
 */

void    HttpResponse::processRequestPOST()
{
    if (!checkLocConfigAndRequest())
        return ;
    if (!_request.isSupportedMediaPOST())
    {
        setStatusCode(UNSUPPORTED_MEDIA_TYPE);
        return ;
    }
    if (_request.getHeaders().find("content-length") == _request.getHeaders().end())
    {
        setStatusCode(BAD_REQUEST);
        return ;
    }
    
    std::string type = _request.getHeader("content-type");
    if (type == "application/x-www-form-urlencoded")
    {
        if (isRedirect())
        {
            getRedirectContent();
            return ;
        }
    }
    std::string boundary;
    else if (isMultiPartFormData(&boundary))
    {
        uploadMultipartForm(boundary);
    }
}

void    HttpResponse::parseMultipartForm(const std::string &boundary)
{
    if (_request.)
}

void    HttpResponse::uploadMultipartForm(const std::string &boundary)
{
    parseMultipartForm(boundary);
}

bool    HttpResponse::isMultiPartFormData(std::string *boundary)
{
    std::string type = _request.getHeader("content-type");
    size_t pos = type.find("multipart/form-data");
    if (pos == std::string::npos)
        return (false);
    pos = type.find("boundary=");
    if (pos == std::string::npos)
        return (false);
    *boundary = type.substr(pos + 9);
    return (true);
}