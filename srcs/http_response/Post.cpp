/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Post.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 10:57:50 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/21 23:23:01 by jdagoy           ###   ########.fr       */
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
#include <iterator>


void    HttpResponse::processRequestPOST()
{
    _serverConfig = checkLocConfigAndRequest();
    if (!_serverConfig.isValid())
    {
        if (_status == INIT)
            setStatusCode(INTERNAL_SERVER_ERROR);
        return ;
    }

    _locationConfig = getLocationConfig();
    if (_locationConfig.getPath().empty())
    {
        setStatusCode(NOT_FOUND);
        return ;
    }
    
    if (!isMethodAllowed(_locationConfig, _request.getRequestLine().getMethod()))
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
        if (isRedirect(_locationConfig))
        {
            getRedirectContent();
            return ;
        }
    }
    else if (_request.isMultiPartFormData())
    {
        if (_request.isForUpload())
        {
            std::cout << "Processing image upload" << std::endl;
            processImageUpload();
        }
    }
}

void HttpResponse::processImageUpload()
{
    std::srand(static_cast<unsigned>(std::time(0)));
    
    std::cout << "Processing image upload" << std::endl;
    std::string directory = "./website/directory/uploads/";
    struct stat st;
    if (stat(directory.c_str(), &st) == -1 || !S_ISDIR(st.st_mode))
    {
        setStatusCode(INTERNAL_SERVER_ERROR);
        return ;
    }

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
            std::string filename = it->second.fields["filename"];
            if (filename.empty())
            {
                setStatusCode(INTERNAL_SERVER_ERROR);
                return ;
            }
            
            std::string filepath = directory + filename;
            std::ofstream filestream(filepath.c_str(), std::ios::binary);
            if (!filestream.is_open())
            {
                setStatusCode(INTERNAL_SERVER_ERROR);
                return ;
            }
            if (it->second.binary.empty())
            {
                setStatusCode(BAD_REQUEST);
                return ;
            }
            filestream.write(reinterpret_cast<const char*>(&it->second.binary[0]), it->second.binary.size());
            if (filestream.fail())
            {
                setStatusCode(INTERNAL_SERVER_ERROR);
                return ;
            }
            filestream.close();
            setStatusCode(CREATED);
        }
    }
}
