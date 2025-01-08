/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagz28 <jdagz28@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 02:24:09 by jdagz28           #+#    #+#             */
/*   Updated: 2025/01/08 13:21:53 by jdagz28          ###   ########.fr       */
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

void    Server::clearSockets()
{
    std::map<socketFD, Socket *>::iterator it;

    for (it = _sockets.begin(); it != _sockets.end(); it++)
    {
        std::cout << "Closing socket: " << it->first << std::endl; //! DELETE
        delete it->second;
        close(it->first);
    }
}

void    Server::initServer()
{
    createSockets();
    setSignals();
}

void    Server::createSockets()
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
            _serverStatus = -1;
            std::cerr << e.what() << std::endl;
        }
    }
}

void    Server::signalHandler(int signum)
{
    std::cout << "Received signal " << signum << std::endl;
    std::cout << "===== Shutting down server =====" << std::endl;
    clearSockets();
    exit(signum);
}

void    Server::setSignals()
{
    try
    {
        if (signal(SIGINT, signalHandler) == -1)
            throw ServerException("Error: Failed to set signal handler for SIGINT");
        if (signal(SIGTERM, signalHandler) == -1)
            throw ServerException("Error: Failed to set signal handler for SIGTERM");
    }
    catch(const std::exception& e)
    {
        _serverStatus = -1;
        std::cerr << e.what() << std::endl;;
    }
}

Server::ServerException::~ServerException()
{}

const char *Server::ServerException::what() const throw()
{
    return (_exceptMsg.c_str());
}