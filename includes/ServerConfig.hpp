/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 02:19:38 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/02 23:14:06 by jdagoy           ###   ########.fr       */
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
        std::vector<std::string>                            _address;
        std::vector<std::string>                            _serverName;
        std::vector<LocationConfig>                         _locationConfig;
        std::map<int, std::string>                          _errorPages;

    public:
        ServerConfig();
        ServerConfig(const ServerConfig &copy);
        ServerConfig  &operator=(const ServerConfig &copy);
        ~ServerConfig();
        
        void    setDirective(const std::string &directive, const std::string &value);
        void    setPort(const std::string &address);
        void    setServerName(const std::string &name);
        void    setLocationConfig(const LocationConfig &locationConfig);
        void    setErrorPage(int errorCode, const std::string &errorPagePath);

        const std::map<std::string, std::vector<std::string> > &getDirectives() const;
        // int   getPort() const;
        const std::string &getServerName() const;
        const std::vector<LocationConfig> &getLocationConfig() const;
        const std::string getErrorPage(StatusCode status) const;
};

#endif