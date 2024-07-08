/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 22:38:46 by jdagoy            #+#    #+#             */
/*   Updated: 2024/07/07 02:47:22 by jdagoy           ###   ########.fr       */
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
        std::pair<int, std::string>             _error;
        std::vector<ServerConfig>               _serverConfig;
        int                                     _serverCount;
        
        Config(const Config &copy);
        Config  &operator=(const Config &copy); 
        
        bool    validPath(const std::string &configPath);
        bool    validExtension(const std::string &configPath);
        void    checkFile(const std::string &configPath);
        void    parseConfig(const std::string &configFile);
        void    parseServerBlock(std::ifstream &infile, ServerConfig &serverConfig);
        void    parseLocationBlock(std::ifstream &infile, LocationConfig &locationConfig);

    public:
        Config(const std::string &configPath);
        ~Config();

        const std::vector<ServerConfig>& getServerConfig() const;

        class configException : public std::exception
        {
            private:
                std::string exceptMsg;
            public:
                configException(const std::string &msg) : exceptMsg(msg) {};
                ~configException() throw() {};
                const char *what() const throw();
        };

};

#endif 