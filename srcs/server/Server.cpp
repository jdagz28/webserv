/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagz28 <jdagz28@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 02:24:09 by jdagz28           #+#    #+#             */
/*   Updated: 2025/01/10 14:59:04 by jdagz28          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <iostream>


Server::Server(const Config &config) 
    : _serverStatus(0), _config(config), _monitoredFDs(), _log(), _readFDs()
{
    _log.checkConfig(config);
}

Server::~Server()
{
    // clear _clients
    clearSockets();
}

void    Server::clearSockets()
{
    std::map<socketFD, Socket *>::iterator it;

    for (it = _monitoredFDs.begin(); it != _monitoredFDs.end(); it++)
    {
        std::cout << "Closing socket: " << it->first << std::endl; //! DELETE
        delete it->second;
        close(it->first);
    }
}

void    Server::initServer()
{
    createSockets();

    std::map<socketFD, Socket *>::iterator it;
    for (it = _monitoredFDs.begin(); it != _monitoredFDs.end(); it++)
        it->second->listenSocket();
    // setSignals();
}

void    Server::createSockets()
{
    std::vector<ServerConfig> servers = _config.getServerConfig();
    std::vector<ServerConfig>::iterator it;

    for (it = servers.begin(); it != servers.end(); it++)
    {
        try
        {
            Socket *socket = new Socket(it->getIP(), it->getPort());
            if (_monitoredFDs.size() <= MAX_CLIENTS)
                _monitoredFDs[socket->getSocketFD()] = socket;
        }
        catch(const std::exception& e)
        {
            _serverStatus = -1;
            std::cerr << e.what() << std::endl;
        }
    }
}

// void    Server::signalHandler(int signum)
// {
//     std::cout << "Received signal " << signum << std::endl;
//     std::cout << "===== Shutting down server =====" << std::endl;
//     clearSockets();
//     exit(signum);
// }

// void    Server::setSignals()
// {
//     try
//     {
//         if (signal(SIGINT, signalHandler) == SIG_ERR)
//             throw ServerException("Error: Failed to set signal handler for SIGINT");
//         if (signal(SIGTERM, signalHandler) == SIG_ERR)
//             throw ServerException("Error: Failed to set signal handler for SIGTERM");
//     }
//     catch(const std::exception& e)
//     {
//         _serverStatus = -1;
//         std::cerr << e.what() << std::endl;
//     }
// }

const char *Server::ServerException::what() const throw()
{
    return (_exceptMsg.c_str());
}

void    Server::runServer()
{
    if (_serverStatus == -1)
        return;

    try
    {
        addMasterFD();
        handleConnections();
    }
    catch (const std::exception& e)
    {
        _serverStatus = -1;
        std::cerr << e.what() << std::endl;
    }
}

void    Server::addMasterFD()
{   
    std::map<socketFD, Socket *>::iterator it;
    for (it = _monitoredFDs.begin(); it != _monitoredFDs.end(); it++)
    {
        if (_monitoredFDs.size() <= MAX_CLIENTS)
            FD_SET(it->first, &_readFDs);
    }
}

void    Server::reInitMonitoredFDs()
{
    std::map<socketFD, Socket *>::iterator it;
    for (it = _monitoredFDs.begin(); it != _monitoredFDs.end(); it++)
    {
        if (_monitoredFDs.size() <= MAX_CLIENTS)
            FD_SET(it->first, &_readFDs);
    }
}

int Server::getMaxFD()
{
    int max = -1;
    
    std::map<socketFD, Socket *>::iterator it;
    for (it = _monitoredFDs.begin(); it != _monitoredFDs.end(); it++)
    {
        if (it->first > max)
            max = it->first;
    }

    return (max);
}

void    Server::handleConnections()
{
    while (true)
    {
        reInitMonitoredFDs();
        int maxFD = getMaxFD();

        int activity = select(maxFD + 1, &_readFDs, NULL, NULL, NULL);
        if (activity == -1)
        {
            _serverStatus = -1;
            throw ServerException("Error: select failed");
        }


        // Check for activity on server sockets
        std::map<socketFD, Socket *>::iterator it;
        for (it = _monitoredFDs.begin(); it != _monitoredFDs.end(); it++)
        {
            try 
            {
                clientFD client = it->second->acceptSocket();
                _clients[client] = it->second;

                // Handle client connection
                HttpRequest request(client);
                HttpResponse response(request, _config, client);
                response.sendResponse();

                close(client); 
            } 
            catch (const std::exception& e)
            {
                _serverStatus = -1;
                std::cerr << e.what() << std::endl;
            }
        }
    }
}
