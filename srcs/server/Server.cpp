/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagz28 <jdagz28@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 02:24:09 by jdagz28           #+#    #+#             */
/*   Updated: 2025/01/09 12:32:55 by jdagz28          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <iostream>


Server::Server(const Config &config) 
    : _serverStatus(0), _config(config), _sockets()
{
}

Server::~Server()
{
    // clear _clients
    clearSockets();
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

    std::map<socketFD, Socket *>::iterator it;
    for (it = _sockets.begin(); it != _sockets.end(); it++)
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
            _sockets[socket->getSocketFD()] = socket;
            std::cout << "Created socket: " << socket->getSocketFD() << std::endl; //! DELETE
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
        handleConnections();
    }
    catch (const std::exception& e)
    {
        _serverStatus = -1;
        std::cerr << e.what() << std::endl;
    }
}

void    Server::handleConnections()
{
    while (true)
    {
        fd_set readFDs;
        int maxFD = 0;

        std::map<socketFD, Socket *>::iterator it;
        for (it = _sockets.begin(); it != _sockets.end(); it++)
        {
            FD_SET(it->first, &readFDs);
            if (it->first > maxFD)
                maxFD = it->first;
        }

        int activity = select(maxFD + 1, &readFDs, NULL, NULL, NULL);
        if (activity == -1)
        {
            _serverStatus = -1;
            throw ServerException("Error: select failed");
        }


        // Check for activity on server sockets
        for (it = _sockets.begin(); it != _sockets.end(); it++)
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
