/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 02:19:38 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/11 23:01:22 by jdagoy           ###   ########.fr       */
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
        int                                                 _port;
        std::string                                         _serverName;
        std::vector<LocationConfig>                         _locationConfig;

    public:
        ServerConfig();
        ServerConfig(const ServerConfig &copy);
        ServerConfig  &operator=(const ServerConfig &copy);
        ~ServerConfig();
        
        void    setDirective(const std::string &directive, const std::string &value);
        void    setPort(const std::string &port);
        void    setServerName(const std::string &name);
        void    setLocationConfig(const LocationConfig &locationConfig);

        const std::map<std::string, std::vector<std::string> > &getDirectives() const;
        int   getPort() const;
        const std::string &getServerName() const;
        const std::vector<LocationConfig> &getLocationConfig() const;
        const std::string getErrorPage(StatusCode status) const;
};

#endif