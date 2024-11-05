/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 10:16:19 by jdagoy            #+#    #+#             */
/*   Updated: 2024/11/05 13:01:34 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
#include <iostream>
#include "Config.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"



//https://betterstack.com/community/guides/logging/how-to-view-and-configure-nginx-access-and-error-logs/

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

        void generateAccessLog();
};

#endif