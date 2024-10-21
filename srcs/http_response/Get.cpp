/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Get.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 01:05:38 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/21 12:27:18 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "webserv.hpp"
#include "ServerConfig.hpp"
#include "HttpRequestLine.hpp"
#include "LocationConfig.hpp"
#include <string>
#include <sys/stat.h>
#include <dirent.h>
#include <ctime>
#include <sstream>
#include <set>

void HttpResponse::processRequestGET()
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
    
    if (isDirectory(path))
    {
        if (path.find("directory") != std::string::npos)
        {
            std::string uri = _request.getRequestLine().getUri();
            size_t pos = uri.find("directory/");
            std::string checkPath = path + uri.substr(pos + 10);
            while (checkPath.find("//") != std::string::npos)
                checkPath.erase(checkPath.find("//"), 1);
            if (fileExists(checkPath))
            {
                getResourceContent(checkPath);
            }
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
        std::string defaultPage = getDirectiveLoc("index");
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
    std::string index = getDirectiveLoc("index");
    if (index.empty())
        return (false);
    return (true);
}

bool HttpResponse::isAutoIndex()
{
    std::string autoIndex = getDirectiveLoc("autoindex");
    if (autoIndex.empty() || autoIndex != "on")
        return (false);
    return (true);
}

std::string getTimeStamp(time_t time)
{
    char formatedTime[30];
    struct tm *time_info;

    time_info = std::localtime(&time); 
    
    if (time_info->tm_isdst > 0)
    {
        std::strftime(formatedTime, 30, "%Y-%m-%d %H:%M CEST", time_info);
        time_info->tm_hour += 2;
    }
    else
    {
        std::strftime(formatedTime, 30, "%Y-%m-%d %H:%M CET", time_info);
        time_info->tm_hour += 1;
    }
    
    return (std::string(formatedTime));
}

void HttpResponse::generateDirList(std::string path)
{
    std::set<FileData> directories;
    std::set<FileData> files;
    DIR *dir;
    struct dirent *dirEntry_ptr;
    struct stat statbuf;
    struct FileData fileInfo;
    
    //OpenDir
    std::string uri = _request.getRequestLine().getUri();
    while (uri.find("//") != std::string::npos)
        uri.erase(uri.find("//"), 1);
    if (path.find(uri) == std::string::npos)
    {
        size_t addPath = uri.find("directory/");
        path = path + uri.substr(addPath + 10);
        if (path[path.size() - 1] == '/')
            path = path.substr(0, path.size() - 1);
    }
    dir = opendir(path.c_str());
    if (dir == NULL)
    {
        setStatusCode(INTERNAL_SERVER_ERROR);
        return ;
    }

    //ReadDir
    while (true)
    {
        dirEntry_ptr = readdir(dir);
        if (dirEntry_ptr == NULL)
        {
            setStatusCode(INTERNAL_SERVER_ERROR);
            break ;
        }
        std::string filename = dirEntry_ptr->d_name;
        if (filename == "." || filename == "..")
            continue ;
        std::string filepath = path + "/" + filename;
        
        int statRes = stat(filepath.c_str(), &statbuf);
        if (statRes == -1)
        {
            setStatusCode(INTERNAL_SERVER_ERROR);
            break ;
        }
        fileInfo.filename = filename;
        fileInfo.size = statbuf.st_size;
        fileInfo.lastModified = getTimeStamp(statbuf.st_mtime);
        if (S_ISDIR(statbuf.st_mode))
        {
            fileInfo.contentType = "directory";
            directories.insert(fileInfo);
        }
        else
        {
            fileInfo.contentType = getExtension(filename);
            files.insert(fileInfo);
        } 
    }
    closedir(dir);
    
    _headers["Location"] = _request.getHost() + "/" + path;
    std::stringstream html;
    
    html << "<!DOCTYPE html>";
    html << "<html lang=\"en\">";
    
    html << "<head>";
    html << "<meta charset=\"UTF-8\">";
    html << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
    html << "<link rel=\"stylesheet\" href=\"/resources/css/styles.css\">";
    html << "<link rel=\"stylesheet\" href=\"/resources/css/directory.css\">";
    html << "<title>Index of " << path << "</title>";
    html << "</head><body>";

    html << "<nav class=\"navbar\">";
    html << "<ul class=\"nav-list\">";
    html << "<li class=\"nav-item\"><a href=\"/\">Home</a></li>";
    html << "<li class=\"nav-item\"><a href=\"/html/features.html\">Features</a></li>";
    html << "<li class=\"nav-item\"><a href=\"/directory\">Directory</a></li>";
    html << "<li class=\"nav-item\"><a href=\"/html/search.html\">Search</a></li>";
    html << "<li class=\"nav-item\"><a href=\"/\">About Us</a></li>";
    html << "</ul>";
    html << "</nav><div class=\"directory\">";
    
    html << "<h1>Index of " << path << "</h1>";

    html << "<table class=\"table\">";
    html << "<thead>";
    html << "<tr><th>Name</th><th>Size</th><th>Last Modified</th></tr>";
    html << "</thead><tbody>";

    std::set<FileData>::iterator itDir;
    for (itDir = directories.begin(); itDir != directories.end(); ++itDir)
    {
        html << "<tr>";
        html << "<td><a href=\"" + _request.getRequestLine().getUri() + "/" + itDir->filename + "/\">" + itDir->filename + "</a></td>";
        html << "<td>--</td>";  
        html << "<td>" + itDir->lastModified + "</td>";
        html << "</tr>";
    }

    std::set<FileData>::iterator itFiles;
    for (itFiles = files.begin(); itFiles != files.end(); ++itFiles)
    {
        html << "<tr>";
        html << "<td><a href=\"" + _request.getRequestLine().getUri() + "/" + itFiles->filename + "\">" + itFiles->filename + "</a></td>";
        html << "<td>" + toString(itFiles->size) + " bytes</td>"; 
        html << "<td>" + itFiles->lastModified + "</td>";
        html << "</tr>";
    }

    html << "</tbody></table>";
    html << "</div></body></html>";
    
    _body = html.str();
    setStatusCode(OK);
}
