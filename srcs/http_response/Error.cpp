/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 22:50:55 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/11 23:29:31 by jdagoy           ###   ########.fr       */
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
    std::cout << "Error Page Path: " << ErrorPagePath << std::endl;
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
std::string page =
        std::string("<!DOCTYPE html>\n") +
        "<html lang=\"en\">\n" +
        "<head>\n" +
        "    <meta charset=\"UTF-8\">\n" +
        "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n" +
        "    <title>" + status + " " + statusMessage + "</title>\n" +
        "    <style>\n" +
        "        body {\n" +
        "            font-family: \"Helvetica Neue\", Helvetica, Arial, sans-serif;\n" +
        "            text-align: center;\n" +
        "            margin-top: 50px;\n" +
        "        }\n" +
        "        h1 {\n" +
        "            color: darkslategray;\n" +
        "        }\n" +
        "    </style>\n" +
        "</head>\n" +
        "<body>\n" +
        "    <h1>" + status + ": " + statusMessage + "</h1>\n" +
        "</body>\n" +
        "</html>\n";

    return (page);   
}




