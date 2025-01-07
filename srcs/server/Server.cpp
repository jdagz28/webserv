/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagz28 <jdagz28@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 02:24:09 by jdagz28           #+#    #+#             */
/*   Updated: 2025/01/08 09:16:31 by jdagz28          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <iostream>


Server::Server(const Config &config) 
    : _config(config), _serverStatus(0), _sockets()
{
}

Server::~Server()
{
    // clear _clients
    // clear _sockets
}

void    Server::initServer()
{
    create_sockets();
}

void    Server::create_sockets()
{
    const std::vector<ServerConfig> &servers = _config.getServerConfig();
    const std::vector<ServerConfig>::const_iterator &it;

    for (it = servers.begin(); it != servers.end(); it++)
    {
        try
        {
            Socket *socket = new Socket(it->getIP(), it->getPort());
            _sockets[socket->getSocketFD()] = socket;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
}