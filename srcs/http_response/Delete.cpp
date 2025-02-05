/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Delete.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 21:37:25 by jdagoy            #+#    #+#             */
/*   Updated: 2025/02/03 16:02:43 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "webserv.hpp"
#include <string>
#include <map>

void HttpResponse::deleteFile(const std::string &file)
{
    std::string path = resolvePath(_serverConfig);
    if (path.empty())
    {
        setStatusCode(NOT_FOUND);
        return ;
    }
    std::string indexPath = buildResourcePath(path, file);
    if (fileExists(indexPath))
    {
        if (remove(indexPath.c_str()) != 0)
            setStatusCode(INTERNAL_SERVER_ERROR);
    }
    else
        setStatusCode(NOT_FOUND);
}

void HttpResponse::curlDelete()
{
    std::string uri = cleanURI(_request.getRequestLine().getUri());
    size_t questionPos = uri.find('?');
    if (questionPos != std::string::npos)
    {
        std::string queryStr = uri.substr(questionPos + 1);
        size_t equalPos = queryStr.find('=');
        if (equalPos != std::string::npos)
        {
            std::string key = queryStr.substr(0, equalPos);
            std::string file = queryStr.substr(equalPos + 1);
            deleteFile(file);
        }
        else
        {
            setStatusCode(BAD_REQUEST);
            return ;
        }
    }
}


void HttpResponse::processRequestDELETE()
{
    std::map<std::string, std::string> toDelete = _request.getFormData();
    if (toDelete.empty())
    {
        curlDelete();
        if (getStatusCode() != OK && getStatusCode() != INIT)
            return ;
    }
    else
    {
        std::map<std::string, std::string>::iterator it;
        for (it = toDelete.begin(); it != toDelete.end(); it++)
        {
            if (it->first.find("files") == std::string::npos)
            {
                setStatusCode(BAD_REQUEST);
                return ;
            }
            std::string file = it->second;
            deleteFile(file);
            if (getStatusCode() != OK && getStatusCode() != INIT)
                return ;
        }
    }
    setStatusCode(SEE_OTHER);
    _headers["Location"] = "/directory/uploads";
}
