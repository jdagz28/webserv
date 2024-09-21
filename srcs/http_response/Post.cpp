/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Post.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 10:57:50 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/21 23:12:20 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "webserv.hpp"
#include "ServerConfig.hpp"
#include "HttpRequestLine.hpp"
#include "LocationConfig.hpp"
#include <string>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sys/stat.h>


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
    if (_request.getStatusCode() >= 400)
    {
        setStatusCode(_request.getStatusCode());
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
    else if (_request.isMultiPartFormData())
    {
        if (_request.isForUpload())
        {
            processImageUpload();
        }
    }
}


std::string HttpResponse::generateFilename(const std::string &extension)
{
    std::time_t now = std::time(NULL);
    std::tm *tm = std::localtime(&now);

    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", tm);
    std::string timestamp(buffer);

    std::string filename = "image_" + timestamp + "." + extension;

    return (filename);
}


void HttpResponse::processImageUpload()
{
    std::srand(static_cast<unsigned>(std::time(0)));
    
    std::string directory = "./website/uploads/";
    struct stat st;
    if (stat(directory.c_str(), &st) == -1 || !S_ISDIR(st.st_mode))
    {
        setStatusCode(INTERNAL_SERVER_ERROR);
        return ;
    }

    std::cout << "test" << std::endl;
    std::map<std::string, MultiFormData> formData = _request.getMultiFormData();
    std::map<std::string, MultiFormData>::iterator it;
    for (it = formData.begin(); it != formData.end(); it++)
    {
        std::string type = it->second.fields["content-type"];
        if (type.empty())
            break ;
        if (_request.isSupportedMediaPOST(type))
        {
            size_t slashPos = type.find("/");
            if (slashPos == std::string::npos)
                return ;
            std::string extension = type.substr(slashPos + 1);
            std::string filename = generateFilename(extension);
            if (filename.empty())
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
            std::cout << "UPLOADING." << std::endl;
            filestream.write(reinterpret_cast<const char *>(&it->second.binary[0]), it->second.binary.size());
            filestream.close();
            setStatusCode(CREATED);
            return ;
        }
    }
}
