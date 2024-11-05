/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 10:15:02 by jdagoy            #+#    #+#             */
/*   Updated: 2024/11/05 11:45:26 by jdagoy           ###   ########.fr       */
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
    
    std::string message = YELLOW + "==========================================\n";
    message += "📡 REQUEST\n";
    message += line.getMethod() + " " + line.getUri() + " " + line.getVersion() + "\n";
    message += "Host: " + request.getHost() + "\n";
    message += "==========================================\n" + RESET;
    std::cout << message;
}