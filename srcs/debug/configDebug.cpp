/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configDebug.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/07 01:23:08 by jdagoy            #+#    #+#             */
/*   Updated: 2024/07/07 02:48:21 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include <iostream>


void    printConfigData(const Config &config)
{
    for (size_t i = 0; i < config.getServerConfig().size(); i++)
    {
        const ServerConfig &server = config.getServerConfig()[i];

        std::cout << "=======================================" << std::endl;
        std::cout << "Server Name: " << server.getServerName() << std::endl;
        std::cout << "Port: " << server.getPort() << std::endl << std::endl;

        std::cout << "DIRECTIVES" << std::endl;
        const std::map<std::string, std::vector<std::string> > &directives = server.getDirectives();
        std::map<std::string, std::vector<std::string> >::const_iterator it;
        for (it = directives.begin(); it != directives.end(); ++it)
        {
             std::cout << "\t";
            std::cout << it->first << std::endl;
            const std::vector<std::string> &values = it->second;
            for (size_t j = 0; j < values.size(); j++)
            {
                std::cout << "\t" << "\t";
                std::cout << "Value: " << values[j] << std::endl;
            }
        }
        
    }
}