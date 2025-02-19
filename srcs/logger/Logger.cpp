/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 10:15:02 by jdagoy            #+#    #+#             */
/*   Updated: 2025/01/14 22:45:20 by jdagoy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"
#include <iostream>
#include "HttpRequestLine.hpp"

Logger::Logger()
{}

Logger::~Logger()
{}

void Logger::checkConfig(const Config &config)
{
    std::string message = GREEN + "\n✅ Configuration File Succesfully Loaded.\n" + RESET;
    message += "==========================================\n";
    std::cout << message;
    
    std::vector<ServerConfig> servers = config.getServerConfig();
    std::vector<ServerConfig>::iterator it;
	std::set<int> printedPorts;

    for (it = servers.begin(); it != servers.end(); ++it)
    {
        int port = it->getPort();
        if (printedPorts.find(port) == printedPorts.end())
        {
            printedPorts.insert(port);
            listening(port, &message, config);
        }
    }
    std::string closing = "==========================================\n";
    std::cout << closing;
    message += closing;
    _terminalLog.push_back(message);
}

void Logger::listening(int port, std::string *message, const Config &config)
{
    std::vector<ServerConfig> servers = config.getServerConfig();
    std::vector<ServerConfig>::iterator it;
    for (it = servers.begin(); it != servers.end(); it++)
    {
        if (it->getPort() == port)
        {
            std::string serverName;
            std::vector<std::string> names = it->getServerNames();
            for (size_t i = 0; i < names.size(); i++)
            {
                serverName += names[i];
                if (i + 1 < names.size())
                    serverName += "\t";
            }

            std::string log = "Listening on port: " + GREEN + toString(port) + RESET + "\t\t" + serverName + "\n";
            *message += log;
            std::cout << log;
        }
    }
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
    
    std::string message = "==========================================\n";
    message += YELLOW + "📡 REQUEST\n";
    message += requestLine;
    message += host;
    
    std::cout << message;
    
    _log["request line"] = requestLine;
    _log["host"] = host;
    _log["referer"] = request.getHeader("referer");
    _terminalLog.push_back(message);
}

void Logger::response(HttpResponse &response)
{
    std::string statusLine =  response.getStatusLine();
    std::string contentType = response.getHeader("Content-Type");
    std::string contentLen = response.getHeader("Content-Length");
    std::string date = response.getHeader("Date");
    std::string server = response.getHeader("Server");

    std::string color = GREEN;
    if (response.getStatusCode() >= 400)
        color = RED;

    std::string message = color + "\n🌐 RESPONSE\n";
    message += statusLine;
    message += "Server: " + server + "\n";
    message += "Content-Type: " + contentType + "\n";
    message += "Content-Length: " + contentLen + "\n";
    message += "Date: " + date + "\n" + RESET;
    message += "==========================================\n";
    std::cout << message;

    _log["status line"] =  statusLine;
    _log["server"] = server;
    _log["content-type"] = contentType;
    _log["content-length"] = contentLen;
    _log["date"] = date;
    _terminalLog.push_back(message);
}

void    Logger::acceptedConnection(const sockaddr_in &address, int port)
{
    std::string ip = inet_ntoa(address.sin_addr);
    
    std::string message = "Accepted connection from " + GREEN + ip + RESET + " on port " + GREEN + toString(port) + RESET + "\n";
    std::cout << message;
    _log["client address"] = ip;
    _log["port"] = toString(port);
    _terminalLog.push_back(message);
}