/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagz28 <jdagz28@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 02:24:09 by jdagz28           #+#    #+#             */
/*   Updated: 2025/01/13 13:06:57 by jdagz28          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <iostream>
#include <algorithm>
#include <arpa/inet.h> 
#include <fcntl.h>


Server::Server(const Config &config) 
    : _config(config), _masterFDs(), _monitoredFDs(), _log(), _readFDs()
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
    setSignals();
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
            {
                _monitoredFDs[socket->getSocketFD()] = socket;
                _masterFDs.push_back(socket->getSocketFD());
            }
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
    std::cout << std::endl << "Exiting... Received signal " << signum << std::endl;
    std::cout << "===== Shutting down server =====" << std::endl;
    // clearSockets();
    exit(signum);
}

void    Server::setSignals()
{
    try
    {
        if (signal(SIGINT, signalHandler) == SIG_ERR)
            throw ServerException("Error: Failed to set signal handler for SIGINT");
    }
    catch(const std::exception& e)
    {
        _serverStatus = -1;
        std::cerr << e.what() << std::endl;
    }
}

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
    FD_ZERO(&_readFDs);

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

        int activity = select(getMaxFD() + 1, &_readFDs, NULL, NULL, NULL);
        if (activity == -1)
        {
            _serverStatus = -1;
            throw ServerException("Error: select failed");
        }

        std::map<socketFD, Socket *>::iterator it;
        for (it = _monitoredFDs.begin(); it != _monitoredFDs.end(); it++)
        {
            try 
            {
                std::vector<socketFD>::iterator masterFD;
                masterFD = std::find(_masterFDs.begin(), _masterFDs.end(), it->first);

                if (masterFD != _masterFDs.end() && FD_ISSET(*masterFD, &_readFDs)) 
                {
                    // Create connction
                    clientFD client = it->second->acceptSocket();
                    _monitoredFDs[client] = it->second;
                    // Set client FD to non-blocking mode
                    if (fcntl(client, F_SETFL, O_NONBLOCK) == -1)
                    {
                        close (client);
                        std::cout << "Error: Failed to set client socket to non-blocking mode" << std::endl; //! DELETE
                        throw ServerException("Error: Failed to set client socket to non-blocking mode");
                    }
                    
                    std::cout << "Accepted connection from " << inet_ntoa(it->second->getAddressInfo().sin_addr) 
                            << " on port " << ntohs(it->second->getAddressInfo().sin_port) << std::endl; //! DELETE
                }
                else if (FD_ISSET(it->first, &_readFDs))
                {
                    // Serve client
                    // Find the client that the data has arrived
                    int clientFD = it->first;;
            
                    // Handle client connection
                    HttpRequest request(clientFD);
                    HttpResponse response(request, _config, clientFD);
                    response.sendResponse();
                }
            } 
            catch (const std::exception& e)
            {
                _serverStatus = -1;
                std::cerr << e.what() << std::endl;
            }
            
        }
    }
}
