/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 10:15:02 by jdagoy            #+#    #+#             */
/*   Updated: 2024/12/06 13:15:00 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"
#include <iostream>
#include "HttpRequestLine.hpp"

Logger::Logger()
{
}

Logger::~Logger()
{
}

void Logger::checkConfig(const Config &config)
{
    std::string message = GREEN + "\n✅ Configuration File Succesfully Loaded.\n" + RESET;
    message += "==========================================\n";
    std::cout << message;
    
    std::vector<ServerConfig> servers = config.getServerConfig();
    std::vector<ServerConfig>::iterator it;
    for (it = servers.begin(); it != servers.end(); it++)
    {
        int port = it->getPort();
        listening(port, &message);
    }
    std::string closing = "==========================================\n";
    std::cout << closing;
    message += closing;
    _terminalLog.push_back(message);
}

void Logger::listening(int port, std::string *message)
{
    std::string log = "Listening on port: " + GREEN + toString(port) + RESET + "\n";
    *message += log;
    std::cout << log;
}

void Logger::configError(const std::string &error)
{
    _terminalLog.push_back(error);
}

void Logger::request(const HttpRequest &request)
{
    HttpRequestLine line = request.getRequestLine();

    std::string requestLine = line.getMethod() + " " + line.getUri() + " " + line.getVersion() + "\n";
    std::string host = "Host: " + request.getHost() + "\n";
    std::string cookies = "Cookies: " + request.printCookies() + "\n";
    
    std::string message = "==========================================\n";
    message += YELLOW + "📡 REQUEST\n";
    message += requestLine;
    message += host;
    if (!(request.printCookies().empty()))
        message += cookies;

    
    std::cout << message;
    
    _log["request line"] = requestLine;
    _log["host"] = host;
    _log["referer"] = request.getHeader("referer");
    if (!(request.printCookies().empty()))
        _log["cookies"] = cookies;
    //! client address 
    _terminalLog.push_back(message);
}

void Logger::response(HttpResponse &response)
{
    std::string statusLine =  response.getStatusLine();
    std::string contentType = response.getHeader("Content-Type");
    std::string contentLen = response.getHeader("Content-Length");
    std::string date = response.getHeader("Date");
    std::string server = response.getHeader("Server");
    std::string cookies = response.getHeader("Set-Cookie");

    std::string color = GREEN;
    if (response.getStatusCode() >= 400)
        color = RED;

    std::string message = color + "\n🌐 RESPONSE\n";
    message += statusLine;
    message += "Server: " + server + "\n";
    message += "Content-Type: " + contentType + "\n";
    message += "Content-Length: " + contentLen + "\n";
    message += "Date: " + date + "\n";
    if (!cookies.empty())
        message += "Set-Cookie: " + cookies + "\n" + RESET;
    message += "==========================================\n";
    std::cout << message;

    _log["status line"] =  statusLine;
    _log["server"] = server;
    _log["content-type"] = contentType;
    _log["content-length"] = contentLen;
    _log["date"] = date;
    if (!cookies.empty())
        _log["cookies"] = cookies;
    _terminalLog.push_back(message);
}