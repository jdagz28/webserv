/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 22:38:46 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/01 11:55:16 by jdagoy           ###   ########.fr       */
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
        std::string                             _error;
        std::vector<ServerConfig>               _serverConfig;
        int                                     _serverCount;
        time_t                                  _keepAliveTimeOut;
        std::map<int, std::string>              _errorPages;
        
        Config(const Config &copy);
        Config  &operator=(const Config &copy); 
        
        bool    validPath(const std::string &configPath);
        bool    validExtension(const std::string &configPath);
        void    checkFile(const std::string &configPath);
        void    parseConfig(const std::string &configFile);
        void    parseServerBlock(std::ifstream &infile, ServerConfig &serverConfig);
        void    parseLocationBlock(std::ifstream &infile, LocationConfig &locationConfig);

        void    skipEventsBlock(std::ifstream &infile);
        void    checkBraces(const std::string &token, int &braceCount, bool &hasOpeningBrace, bool &hasClosingBrace);

        // http-block parsing
        void    parseHttpBlock(std::ifstream &infile);
        void    parseHttpDirective(const std::string &token, std::istringstream &iss, std::ifstream &infile);
        void    parseErrorPages(std::istringstream &iss);
        void    parseKeepAlive(std::istringstream &iss);

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
            public:
                configException(const std::string &msg)
                {
                    exceptMsg = "Error: " + msg;
                };
                ~configException() throw() {};
                const char *what() const throw();
        };

};

#endif 