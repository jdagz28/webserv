/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Post.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 10:57:50 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/19 09:19:12 by jdagoy           ###   ########.fr       */
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
    _request.parseRequestBody();
    if (_request.getStatusCode() != OK || _request.getStatusCode() != INIT)
    {
        setStatusCode(_request.getStatusCode());
        return ;
    }
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
    // else if (isMultiPartFormData(&boundary))
    // {
    //     uploadMultipartForm(boundary);
    // }
}

/*
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
*/