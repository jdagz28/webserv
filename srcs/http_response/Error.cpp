/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 22:50:55 by jdagoy            #+#    #+#             */
/*   Updated: 2025/03/09 01:33:37 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "webserv.hpp"
#include "ServerConfig.hpp"
#include "HttpRequestLine.hpp"
#include "LocationConfig.hpp"
#include <string>
#include <sstream>


void	HttpResponse::getErrorPage()
{
    std::string ErrorPagePath = _serverConfig.getErrorPage(_status);

    if (!ErrorPagePath.empty())
        getResourceContent(ErrorPagePath);
    else
    {
        std::string status = toString(static_cast<int>(_status));
        std::string statusMessage = getStatusReason(_status);
        generateErrorPage(status, statusMessage);
    }
}

void	HttpResponse::generateErrorPage(const std::string &status, const std::string &statusMessage)
{
	std::ostringstream html; 
	
	html << "<!DOCTYPE html>\r\n";
    html << "<html lang=\"en\">\r\n\r\n";
    html << "<head>\r\n";
    html << "\t<meta charset=\"UTF-8\">\r\n";
    html << "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n";
    html << "\t<link rel=\"stylesheet\" href=\"/resources/css/styles.css\">\r\n";
    html << "\t<link rel=\"stylesheet\" href=\"/resources/css/navbar.css\">\r\n";
    html << "\t<title>" + status + " " + statusMessage +  "</title>\r\n";
    html << "</head>\r\n\r\n";
    html << "<body>\r\n";
    
	if (_locationConfig.getRoot() == "website" && _serverConfig.getPort() == 4242)
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
	
    html << "\t<h1 class=\"landing\">" + status + ": " + statusMessage + "</h1>\r\n\r\n";
    html << "</body>\r\n\r\n";
    html << "</html>\r\n";

    _body = html.str(); 
	_headers["Content-Type"] = "text/html";
}




