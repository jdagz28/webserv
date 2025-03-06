/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Post.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 10:57:50 by jdagoy            #+#    #+#             */
/*   Updated: 2025/03/04 14:47:11 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "webserv.hpp"
#include <string>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sys/stat.h>
#include <iterator>
#include <algorithm>


void    HttpResponse::processRequestPOST()
{
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
		processForm();
    }
    else if (_request.isMultiPartFormData())
    {
        if (_request.isForUpload())
            processImageUpload();
    }
}

static std::string appendTimestamp(const std::string &directory, std::string filename)
{
	std::time_t now = std::time(NULL);
    std::tm *ltm = std::localtime(&now);
    char timeStr[16]; 
    std::strftime(timeStr, sizeof(timeStr), "%Y%m%d%H%M%S", ltm);

	size_t pos = filename.find_last_of('.');
	if (pos != std::string::npos)
	{
		std::string baseName = filename.substr(0, pos);
		std::string fileExt = filename.substr(pos);
		return (directory + baseName + "_" + timeStr + fileExt);
	}
	else
		filename += std::string("_") + timeStr;
	return (directory + filename);
}

void	HttpResponse::processImageUpload()
{
    std::srand(static_cast<unsigned>(std::time(0)));
    
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
			struct stat stFile;
			if (stat(filepath.c_str(), &stFile) == 0)
				filepath = appendTimestamp(directory, filename);
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

bool	HttpResponse::checkPostLocation()
{
	std::string uri = _request.getRequestLine().getUri();
	if (_locationConfig.isMethodAllowed("POST"))
	{
		std::string path = _locationConfig.getPath();
		if (uri[uri.size() - 1] == '/')
			uri = uri.substr(0, uri.size() - 1);
		if (uri == path)
			return (true);
	}
	setStatusCode(BAD_REQUEST);
	return (false);
}

void	HttpResponse::processForm()
{
	std::map<std::string, std::string> formData = _request.getFormData();
	std::map<std::string, std::string>::iterator it;
	for (it = formData.begin(); it != formData.end(); it++)
	{
		std::string key = it->first;
		std::string value = it->second;
		if (key == "message")
		{
			std::string message = cleanMessage(value);
			_body = generatePostMessage(message);
			setStatusCode(OK);
			return ;
		}
	}
	setStatusCode(BAD_REQUEST);
}

std::string	HttpResponse::generatePostMessage(const std::string &message)
{
	std::string html = std::string("<!DOCTYPE html>\r\n")
        + "<html lang=\"en\">\r\n\r\n"
        + "<head>\r\n"
        + "\t<meta charset=\"UTF-8\">\r\n"
        + "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n"
        + "\t<link rel=\"stylesheet\" href=\"/resources/css/styles.css\">\r\n"
        + "\t<link rel=\"stylesheet\" href=\"/resources/css/navbar.css\">\r\n"
        + "\t<title>POST Message</title>\r\n"
        + "</head>\r\n\r\n"
        + "<body>\r\n"
        + "\t<nav class=\"navbar\">\r\n"
        + "\t\t<ul class=\"nav-list\">\r\n"
        + "\t\t\t<li class=\"nav-item\"><a href=\"/\">Home</a></li>\r\n"
        + "\t\t\t<li class=\"nav-item\"><a href=\"/html/features.html\">Features</a></li>\r\n"
        + "\t\t\t<li class=\"nav-item\"><a href=\"/directory\">Directory</a></li>\r\n"
        + "\t\t\t<li class=\"nav-item\"><a href=\"/html/search.html\">Search</a></li>\r\n"
        + "\t\t\t<li class=\"nav-item\"><a href=\"/html/about.html\">About Us</a></li>\r\n"
        + "\t\t</ul>\r\n"
        + "\t</nav>\r\n\r\n"
        + "\t<h1 class=\"landing\">" + message + "</h1>\r\n\r\n"
        + "</body>\r\n\r\n"
        + "</html>\r\n";

    return (html);   	
}