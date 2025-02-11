/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 22:50:55 by jdagoy            #+#    #+#             */
/*   Updated: 2025/02/11 12:29:26 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "webserv.hpp"
#include "ServerConfig.hpp"
#include "HttpRequestLine.hpp"
#include "LocationConfig.hpp"
#include <string>


void	HttpResponse::getErrorPage()
{
    std::string ErrorPagePath = _serverConfig.getErrorPage(_status);

    if (!ErrorPagePath.empty())
        getResourceContent(ErrorPagePath);
    else
    {
        std::string status = toString(static_cast<int>(_status));
        std::string statusMessage = getStatusReason(_status);
        _body = generateErrorPage(status, statusMessage);
    }
}

std::string	HttpResponse::generateErrorPage(const std::string &status, const std::string &statusMessage)
{
    std::string html = std::string("<!DOCTYPE html>\r\n")
        + "<html lang=\"en\">\r\n\r\n"
        + "<head>\r\n"
        + "\t<meta charset=\"UTF-8\">\r\n"
        + "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n"
        + "\t<link rel=\"stylesheet\" href=\"/resources/css/styles.css\">\r\n"
        + "\t<link rel=\"stylesheet\" href=\"/resources/css/navbar.css\">\r\n"
        + "\t<title> Error " + status + "</title>\r\n"
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
        + "\t<h1 class=\"landing\">" + status + ": " + statusMessage + "</h1>\r\n\r\n"
        + "</body>\r\n\r\n"
        + "</html>\r\n";

    return (html);   
}




