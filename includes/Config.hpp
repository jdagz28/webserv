/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 22:38:46 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/03 15:30:10 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <exception>
#include <utility>
#include <fstream>
#include "ServerConfig.hpp"

#define DEFAULT_CONFIG "./conf/default.conf"


class Config
{
    private:
        std::string                             _configPath;
        int                                     _parsedLine;
        std::string                             _error;
        std::vector<ServerConfig>               _serverConfig;
        int                                     _serverCount;
        time_t                                  _keepAliveTimeOut;
        std::map<StatusCode, std::string>       _errorPages;
        
        Config(const Config &copy);
        Config  &operator=(const Config &copy); 
        
        bool    validPath(const std::string &configPath);
        bool    validExtension(const std::string &configPath);
        void    checkFile(const std::string &configPath);
        void    parseConfig(const std::string &configFile);

        void    parseLocationBlock(std::ifstream &infile, LocationConfig &locationConfig);

        void    skipEventsBlock(std::ifstream &infile);
        void    checkBraces(const std::string &token, int &openingBrace, int &closingBrace);
        bool    checkErrorPage(const std::string &errorPagePath);

        // http-block parsing
        void    parseHttpBlock(std::ifstream &infile);
        void    parseHttpDirective(const std::string &token, std::istringstream &iss, std::ifstream &infile);
        void    parseErrorPages(std::istringstream &iss);
        void    parseKeepAlive(std::istringstream &iss);

        // server-block parsing
        void    parseServerBlock(std::ifstream &infile, ServerConfig &serverConfig);
        void    parseServerDirective(const std::string &token, std::istringstream &iss, std::ifstream &infile, ServerConfig &serverConfig);
        void    parseErrorPages(std::istringstream &iss, ServerConfig &serverConfig);
        void    parseServerListen(const std::string &value, ServerConfig &serverConfig);
        bool    validPort(const std::string &value);
        bool    checkAddr(const std::string &host, const std::string &port);
        void    parseServerName(const std::string &value, ServerConfig &serverConfig);
        bool    checkServerName(const std::string &name);
        
        

    public:
        Config(const std::string &configPath);
        ~Config();

        const std::vector<ServerConfig>& getServerConfig() const;
        time_t  getKeepAliveTimeout() const;
        std::string getErrorPages() const;

        class configException : public std::exception
        {
            private:
                std::string exceptMsg;
                std::string configPath;
                std::string parsedLine;
                mutable std::string errorMsg;
            public:
                configException(const std::string &msg)
                    : exceptMsg(msg), configPath(""), parsedLine(""), errorMsg("") {};
                configException(const std::string &msg, const std::string &configPath, int line)
                    : exceptMsg(msg), configPath(configPath)
                {
                    parsedLine = toString(line);
                };
                ~configException() throw() {};
                const char *what() const throw();
        };

};

#endif 