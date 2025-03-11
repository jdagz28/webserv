/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Get.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 01:05:38 by jdagoy            #+#    #+#             */
/*   Updated: 2025/03/11 14:50:36 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "webserv.hpp"
#include <string>
#include <sys/stat.h>
#include <dirent.h>
#include <ctime>
#include <sstream>
#include <set>

void	HttpResponse::processRequestGET()
{   
    if (isRedirect(_locationConfig))
    {
        getRedirectContent();
        return ;
    }

    std::string path = resolvePath(_serverConfig);
    if (path.empty())
    {
        setStatusCode(NOT_FOUND);
        return ;
    }
	std::string extension = getExtension(_request.getRequestLine().getUri());
	if (isAutoIndex() && extension.empty())
	{
        generateDirList(path);
		return ;
	}
    if (isDirectory(path))
    {
		std::string checkPath = verifyPath(path);
		if (fileExists(checkPath))
			getResourceContent(checkPath);
        getResource(path);
        return ;
    }
    getResource(path);
}

std::string	HttpResponse::verifyPath(std::string path)
{
    std::string uri = cleanURI(_request.getRequestLine().getUri());
    std::string subset = path.substr(path.find('/') + 1);
    size_t pos = uri.find(subset);
    if (pos != std::string::npos)
        return (path + uri.substr(subset.size()));
    return (path);
}

std::string	HttpResponse::buildResourcePath(const std::string &basePath, const std::string &resourceName)
{
    if (!checkSlash(basePath, resourceName))
        return(basePath + '/' + resourceName);
    else if (basePath[basePath.length() - 1] == '/' && resourceName[0] == '/')
        return(basePath + resourceName.substr(1));
    else
        return(basePath + resourceName);
}


void	HttpResponse::getResource(const std::string &target_path)
{
    std::string indexPath;
        
    std::string uri = cleanURI(_request.getRequestLine().getUri());
	std::string resourceName; 
	if (!_locationConfig.getIndex().empty() && getExtension(_request.getRequestLine().getUri()).empty())
		resourceName = _locationConfig.getIndex();
	else
		resourceName = extractResourceName(uri);
	indexPath = buildResourcePath(target_path, resourceName);
	if (fileExists(indexPath))
		getResourceContent(indexPath);
	else
		setStatusCode(NOT_FOUND);
}

void 	HttpResponse::getResourceContent(const std::string &file_path)
{
    if (!fileExists(file_path))
    {
        setStatusCode(NOT_FOUND);
        return;
    }
	if (access(file_path.c_str(), R_OK) != 0)
    {
        setStatusCode(FORBIDDEN);
        return;
    }
    
    std::ifstream infile(file_path.c_str(), std::ios::binary);
    if (!infile.is_open())
    {
        setStatusCode(NOT_FOUND);
        return ;
    }   
    infile.seekg(0, std::ios::end);
    std::streampos fileSize = infile.tellg(); 
    infile.seekg(0, std::ios::beg); 
    _body.resize(fileSize);
    infile.read(&_body[0], fileSize); 
	if (infile.gcount() != fileSize) 
	{
		setStatusCode(INTERNAL_SERVER_ERROR);
		return;
	}
    infile.close();
    
	std::string contentType = getExtension(file_path);
	addContentTypeHeader(contentType);
	if (!(getStatusCode() >= 400 && getStatusCode() < 600))
		setStatusCode(OK);
}

std::string	HttpResponse::extractResourceName(const std::string &uri)
{
    if (uri == "/")
        return (_locationConfig.getIndex());
    size_t  lastSlashPos = uri.find_last_of('/');
    if (lastSlashPos != std::string::npos && lastSlashPos + 1 < uri.length())
        return (uri.substr(lastSlashPos + 1));
    return (uri);
}

bool	HttpResponse::checkSlash(const std::string &defaultLoc, const std::string &page)
{
    bool defaultSlash = !defaultLoc.empty() && defaultLoc[defaultLoc.length() - 1] == '/';
    bool pageSlash = !page.empty() && page[0] == '/';

    if (!defaultSlash && !pageSlash)
        return (false);
    return (true);
}
