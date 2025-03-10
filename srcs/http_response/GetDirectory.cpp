/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetDirectory.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 15:15:53 by jdagoy            #+#    #+#             */
/*   Updated: 2025/03/09 01:27:31 by jdagoy           ###   ########.fr       */
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

bool	HttpResponse::checkDirIndex()
{
    std::string index = getDirectiveLoc("index");
    if (index.empty())
        return (false);
    return (true);
}

bool	HttpResponse::isAutoIndex()
{
    std::string autoIndex = getDirectiveLoc("autoindex");
    if (autoIndex.empty() || autoIndex != "on")
        return (false);
    return (true);
}

std::string	getTimeStamp(time_t time)
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

void	HttpResponse::generateDirPage(const std::string &path, std::set<FileData> &directories, std::set<FileData> &files)
{
    _headers["Location"] = _request.getHost() + "/" + path;
    std::ostringstream html;

   
    html << "<!DOCTYPE html>\r\n";
    html << "<html lang=\"en\">\r\n\r\n";
    html << "<head>\r\n";
    html << "\t<meta charset=\"UTF-8\">\r\n";
    html << "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n";
	if (_locationConfig.getRoot() == "website")
	{
		html << "\t<link rel=\"stylesheet\" href=\"/resources/css/styles.css\">\r\n";
		html << "\t<link rel=\"stylesheet\" href=\"/resources/css/directory.css\">\r\n";
		html << "\t<link rel=\"stylesheet\" href=\"/resources/css/navbar.css\">\r\n";
	}
	html << "\t<title>Index of " << path << "</title>\r\n";
    html << "</head>\r\n\r\n";
    html << "<body>\r\n";
    
    if (_locationConfig.getRoot() == "website")
    {
		html << "\t<nav class=\"navbar\">\r\n";
    	html << "\t\t<ul class=\"nav-list\">\r\n";
    	html << "\t\t\t<li class=\"nav-item\"><a href=\"/\">Home</a></li>\r\n";
        html << "\t\t\t<li class=\"nav-item\"><a href=\"/html/features.html\">Features</a></li>\r\n";
        html << "\t\t\t<li class=\"nav-item\"><a href=\"/directory\">Directory</a></li>\r\n";
        html << "\t\t\t<li class=\"nav-item\"><a href=\"/html/search.html\">Search</a></li>\r\n";
        html << "\t\t\t<li class=\"nav-item\"><a href=\"/html/about.html\">About Us</a></li>\r\n";
		html << "\t\t</ul>\r\n";
		html << "\t</nav>\r\n\r\n";
	}
	
    html << "\t<div class=\"directory\">\r\n";
    html << "\t\t<h1>Index of " << path << "</h1>\r\n";

    if (path == "website/directory/uploads")
    {
        html << "\t\t<form method=\"POST\" action=\"/directory/uploads\">\r\n";
        html << "\t\t<input type=\"hidden\" name=\"_method\" value=\"DELETE\">\r\n";
        html << "\t\t<table class=\"table\">\r\n";
        html << "\t\t\t<thead>\r\n";
        html << "\t\t\t<tr><th>Select</th><th>Name</th><th>Size</th><th>Last Modified</th></tr>\r\n";
        html << "\t\t\t</thead>";
        html << "\t\t\t<tbody>\r\n";

        std::set<FileData>::iterator itFiles;
        for (itFiles = files.begin(); itFiles != files.end(); ++itFiles)
        {
            html << "\t\t\t<tr>\r\n";
            std::string extension = getExtension(itFiles->filename);
            std::string emoji;
            if (extension == "jpg" || extension == "jpeg" || extension == "png" || extension == "bmp" || extension == "gif")
                emoji = "🖼️ ";
            
            html << "\t\t\t\t<td><input type=\"checkbox\" name=\"files\" value=\"" + itFiles->filename + "\"></td>\r\n";
            html << "\t\t\t\t<td><a href=\"" + _request.getRequestLine().getUri() + "/" + itFiles->filename + "\">" + emoji + itFiles->filename + "</a></td>\r\n";
            html << "\t\t\t\t<td>" + toString(itFiles->size) + " bytes</td>\r\n"; 
            html << "\t\t\t\t<td>" + itFiles->lastModified + "</td>\r\n";
            html << "\t\t\t</tr>\r\n";
        }
        html << "\t\t\t</tbody>\r\n";
        html << "\t\t</table>\r\n";

        html << "\t\t<div class=\"button-wrapper\">\r\n";
        html << "\t\t\t<button type=\"submit\" class=\"file-delete-button\">DELETE</button>\r\n";
        html << "\t\t</div>\r\n";
        html << "\t\t</form>\r\n";
    }

    else
    {
        html << "\t\t<table class=\"table\">\r\n";
        html << "\t\t\t<thead>\r\n";
        html << "\t\t\t<tr><th>Name</th><th>Size</th><th>Last Modified</th></tr>\r\n";
        html << "\t\t\t</thead>";
        html << "\t\t\t<tbody>\r\n";

        std::string uri = _request.getRequestLine().getUri();
        if (uri[uri.length() - 1] != '/' || uri.empty())
            uri += "/";

        std::set<FileData>::iterator itDir;
        for (itDir = directories.begin(); itDir != directories.end(); ++itDir)
        {
            html << "\t\t\t<tr>\r\n";
            html << "\t\t\t\t<td><a href=\"" + uri + itDir->filename + "/\">" + "📁 " + itDir->filename + "</a></td>\r\n";
            html << "\t\t\t\t<td>--</td>\r\n";  
            html << "\t\t\t\t<td>" + itDir->lastModified + "</td>\r\n";
            html << "\t\t\t</tr>\r\n";
        }

        std::set<FileData>::iterator itFiles;
        for (itFiles = files.begin(); itFiles != files.end(); ++itFiles)
        {
            html << "\t\t\t<tr>\r\n";
            std::string extension = getExtension(itFiles->filename);
            std::string emoji;
            if (extension == "html")
                emoji = "🌐 ";
            else if (extension == "jpg" || extension == "jpeg" || extension == "png" || extension == "bmp" || extension == "gif")
                emoji = "🖼️ ";
            html << "\t\t\t\t<td><a href=\"" + uri + itFiles->filename + "\">" + emoji + itFiles->filename + "</a></td>\r\n";
            html << "\t\t\t\t<td>" + toString(itFiles->size) + " bytes</td>\r\n"; 
            html << "\t\t\t\t<td>" + itFiles->lastModified + "</td>\r\n";
            html << "\t\t\t</tr>\r\n";
        }

        html << "\t\t\t</tbody>\r\n";
        html << "\t\t</table>\r\n";
    }
    
    html << "\t</div>\r\n";
    html << "</body>\r\n";
    html << "</html>\r\n";
    
    _body = html.str();
    setStatusCode(OK);
	_headers["Content-Type"] = "text/html";
}

void	HttpResponse::generateDirList(std::string path)
{
    std::set<FileData> directories;
    std::set<FileData> files;
    DIR *dir;
    struct dirent *dirEntry_ptr;
    struct stat statbuf;
    struct FileData fileInfo;
    
    std::string uri = cleanURI(_request.getRequestLine().getUri());
    dir = opendir(path.c_str());
    if (dir == NULL)
    {
        setStatusCode(INTERNAL_SERVER_ERROR);
        return ;
    }

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

    generateDirPage(path, directories, files);
}
