/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Get.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 01:05:38 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/13 09:40:17 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "webserv.hpp"
#include "ServerConfig.hpp"
#include "HttpRequestLine.hpp"
#include "LocationConfig.hpp"
#include <string>
#include <dirent.h>
#include <sstream>

/* 
    * STATIC PAGES
    * RESOURCE/FILE (**)
        * error
        * redirect (**)
        * response (**)
    * DIRECTORY
        * error
        * autoindex
        * response
    ! error pages - wget status line ok    
*/

void HttpResponse::processRequestGET()
{
    if (!checkLocConfigAndRequest())
    {
        if (_status == INIT)
            setStatusCode(NOT_FOUND);
        return ;
    }

    if (isRedirect())
    {
        getRedirectContent();
        return ;
    }
    
    std::string path = resolvePath();
    if (path.empty())
    {
        setStatusCode(NOT_FOUND);
        return ;
    }
    if (isDirectory(path))
    {
        if (checkDirIndex())
        {
            std::string redirect = _request.getRequestLine().getUri();
            if (redirect[redirect.length() - 1] != '/')
                redirect += '/';
            std::string index = getDirective("index");
            if (index.empty())
            {
                setStatusCode(NOT_FOUND);
                return ;
            }
            std::string indexPath;
            if (!checkSlash(redirect, index))
                indexPath = redirect + '/' + index;
            else if (redirect[redirect.length() - 1] == '/' && index[0] == '/')
                indexPath = redirect + index.substr(1);
            else
                indexPath = redirect + index;
            getResource(indexPath);
            return ;
        }
        if (isAutoIndex())
        {
            generateDirList(path);
            return ;
        }
        getResource(path);
        return ;
    }
    getResource(path);
}

void HttpResponse::getResource(const std::string &target_path)
{
    std::string indexPath;
        
    std::string uri = _request.getRequestLine().getUri();
    if (isSupportedMedia(uri))
    {
        std::string resourceName = extractResourceName(uri);
        
        if (!checkSlash(target_path, resourceName))
            indexPath = target_path + '/' + resourceName;
        else if (target_path[target_path.length() - 1] == '/' && resourceName[0] == '/')
            indexPath = target_path + resourceName.substr(1);
        else
            indexPath = target_path + resourceName;
        
        if (fileExists(indexPath))
            getResourceContent(indexPath);
        else
            setStatusCode(NOT_FOUND);
    }
    else
    {
        std::string defaultPage = getDirective("default");
        if (defaultPage.empty())
            setStatusCode(NOT_FOUND);
        if (!checkSlash(target_path, defaultPage))
            indexPath = target_path + '/' + defaultPage;
        else
            indexPath = target_path + defaultPage;
        getResourceContent(indexPath);
    }
}

void  HttpResponse::getResourceContent(const std::string &file_path)
{
    std::ifstream   infile;
    std::string     buffer;
    
    infile.open(file_path.c_str(), std::ios::binary);
    if (!infile.is_open())
    {
        setStatusCode(NOT_FOUND);
        return ;
    }
    std::string contentType = getExtension(file_path);
    if (!isSupportedMedia(contentType))
        setStatusCode(UNSUPPORTED_MEDIA_TYPE);
    
    infile.seekg(0, std::ios::end);
    std::streampos fileSize = infile.tellg(); 
    infile.seekg(0, std::ios::beg); 
    _body.resize(fileSize);
    infile.read(&_body[0], fileSize); 
    infile.close();
    addContentTypeHeader(contentType);
    setStatusCode(OK);
}

std::string HttpResponse::extractResourceName(const std::string &uri)
{
    size_t  lastSlashPos = uri.find_last_of('/');
    if (lastSlashPos != std::string::npos && lastSlashPos + 1 < uri.length())
        return (uri.substr(lastSlashPos + 1));
    return (std::string());
}

bool HttpResponse::checkSlash(const std::string &defaultLoc, const std::string &page)
{
    bool defaultSlash = !defaultLoc.empty() && defaultLoc[defaultLoc.length() - 1] == '/';
    bool pageSlash = !page.empty() && page[0] == '/';

    if (!defaultSlash && !pageSlash)
        return (false);
    return (true);
}

bool HttpResponse::checkDirIndex()
{
    std::string index = getDirective("index");
    if (index.empty())
        return (false);
    return (true);
}

bool HttpResponse::isAutoIndex()
{
    std::string autoIndex = getDirective("autoindex");
    if (autoIndex.empty() || autoIndex != "on")
        return (false);
    return (true);
}

void HttpResponse::generateDirList(const std::string &path)
{
    DIR *dir;
    struct dirent *entry;
    std::stringstream html;

    dir = opendir(path.c_str());
    if (dir == NULL)
    {
        setStatusCode(FORBIDDEN);
        return ;
    }
    
    html << "<html><head><title>Index of " << path << "</title></head><body>";
    html << "<h1>Index of " << path << "</h1>";
    html << "<ul>";

    while ((entry = readdir(dir)) != NULL) {
        std::string entryName = entry->d_name;
        if (entryName == "." || entryName == "..") {
            continue;
        }
        html << "<li><a href=\"" << entryName << "\">" << entryName << "</a></li>";
    }
    closedir(dir);
    _body = html.str();
    setStatusCode(OK);
}
