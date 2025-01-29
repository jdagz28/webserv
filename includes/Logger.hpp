/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 10:16:19 by jdagoy            #+#    #+#             */
/*   Updated: 2025/01/14 22:45:12 by jdagoy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <iostream>
#include "Config.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <arpa/inet.h> 


class Logger 
{
    private:
        std::map<std::string, std::string> _log;
        std::vector<std::string>           _terminalLog;

        Logger(const Logger &copy);
        Logger &operator=(const Logger &copy);
    
        void listening(int port, std::string *message);
        
    public:
        Logger();
        ~Logger();

        void checkConfig(const Config &config);
        void configError(const std::string &error);

        void request(const HttpRequest &request);
        void response(HttpResponse &response);
        void acceptedConnection(const sockaddr_in &address, int port);

        //! void generateAccessLog();
};

#endif