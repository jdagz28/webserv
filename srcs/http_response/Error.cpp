/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 22:50:55 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/24 05:13:35 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "webserv.hpp"
#include "ServerConfig.hpp"
#include "HttpRequestLine.hpp"
#include "LocationConfig.hpp"
#include <string>


void HttpResponse::getErrorPage()
{
    std::string ErrorPagePath;
    
    const std::vector<ServerConfig> &serverConfigs = _config.getServerConfig();
    if (serverConfigs.empty())
        return ;
    std::vector<ServerConfig>::const_iterator server;
    for (server = serverConfigs.begin(); server != serverConfigs.end(); server++)
    {
        ErrorPagePath = server->getErrorPage(_status);
    }
    if (!ErrorPagePath.empty())
        getResourceContent(ErrorPagePath);
    else
    {
        std::string status = toString(static_cast<int>(_status));
        std::string statusMessage = getStatusReason(_status);
        _body = generateErrorPage(status, statusMessage);
    }
}

std::string HttpResponse::generateErrorPage(const std::string &status, const std::string &statusMessage)
{
std::string html = std::string("<!DOCTYPE html>")
        + "<html lang=\"en\">"
        + "<head>"
        + "<meta charset=\"UTF-8\">"
        + "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
        + "<link rel=\"stylesheet\" href=\"/resources/css/styles.css\">"
        + "<title> Error " + status + "</title>"
        + "</head><body>"

        + "<nav class=\"navbar\">"
        + "<ul class=\"nav-list\">"
        + "<li class=\"nav-item\"><a href=\"/\">Home</a></li>"
        + "<li class=\"nav-item\"><a href=\"/html/features.html\">Features</a></li>"
        + "<li class=\"nav-item\"><a href=\"/directory\">Directory</a></li>"
        + "<li class=\"nav-item\"><a href=\"/html/search.html\">Search</a></li>"
        + "<li class=\"nav-item\"><a href=\"/\">About Us</a></li>"
        + "</ul>"
        + "</nav>"
        + "<h1 class=\"landing\">" + status + ": " + statusMessage + "</h1>"
        + "</body>\n"
        + "</html>\n";

    return (html);   
}




