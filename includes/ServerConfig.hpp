/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 02:19:38 by jdagoy            #+#    #+#             */
/*   Updated: 2025/01/07 15:53:33 by jdagoy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <vector>
#include <map>
#include "LocationConfig.hpp"
#include "webserv.hpp"


class ServerConfig
{
    private:
        std::map<std::string, std::vector<std::string> >    _directives;
        std::string                                         _ip;
        int                                                 _port;
        std::vector<std::string>                            _serverName;
        std::vector<LocationConfig>                         _locationConfig;
        std::map<int, std::string>                          _errorPages;
        std::vector<std::string>                            _locationPaths;
        bool                                                _valid;

    public:
        ServerConfig();
        ServerConfig(const ServerConfig &copy);
        ServerConfig  &operator=(const ServerConfig &copy);
        ~ServerConfig();
        
        void    setDirective(const std::string &directive, const std::string &value);
        void    setPort(int port);
        void    setIP(const std::string &ip);
        void    setServerName(const std::string &name);
        void    setLocationConfig(const LocationConfig &locationConfig);
        void    setErrorPage(int errorCode, const std::string &errorPagePath);
        void    setLocationPath(const std::string &path);
        void    setValid();

        const std::map<std::string, std::vector<std::string> > &getDirectives() const;
        int getPort() const;
        const std::string getIP() const;
        std::string getServerName() const;
        std::string checkServerName(const std::string &requestHost) const;
        const std::vector<LocationConfig> &getLocationConfig() const;
        LocationConfig getLocationConfig(const std::string &path) const;
        const std::string getErrorPage(StatusCode status) const;
        bool    isPathAlreadySet(const std::string &path) const;
        bool    isValid() const;
};

#endif