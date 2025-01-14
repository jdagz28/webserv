/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagz28 <jdagz28@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 02:24:09 by jdagz28           #+#    #+#             */
/*   Updated: 2025/01/14 22:46:00 by jdagz28          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <iostream>
#include <algorithm>
#include <arpa/inet.h> 
#include <fcntl.h>
#include <sys/epoll.h>
#include <cstring>


Server::Server(const Config &config) 
    : _config(config), _masterFDs(), _monitoredFDs(), _clients(), _log()
{
    _log.checkConfig(config);
}

Server::~Server()
{
    clearClients();
    clearSockets();
}

void   Server::clearClients()
{
    std::map<clientFD, Event *>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); it++)
    {
        delete it->second;
        close(it->first);
    }
    _clients.clear();
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
        handleConnections();
    }
    catch (const std::exception& e)
    {
        _serverStatus = -1;
        std::cerr << e.what() << std::endl;
    }
}

int setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return (-1);
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
        return (-1);
        
    return (0);
}

void    Server::handleConnections()
{
    int epollFD = epoll_create(1);
    if (epollFD == -1)
        throw ServerException("Error: Failed to create epoll instance");
    
    // Add master FDs (listen sockets) to epoll 
    std::vector<socketFD>::iterator it;
    for (it = _masterFDs.begin(); it != _masterFDs.end(); it++)
    {
        if (setNonBlocking(*it) == -1)
            throw ServerException("Error: Failed to set master socket to non-blocking mode");
            
        struct epoll_event event;
        event.data.fd = *it;
        event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;

        if (epoll_ctl(epollFD, EPOLL_CTL_ADD, *it, &event) == -1)
        {
            close(epollFD);
            throw ServerException("Error: Failed to add master socket to epoll instance");
        }
    }

    struct epoll_event events[MAX_CLIENTS];

    while (true)
    {
        int nEvents = epoll_wait(epollFD, events, MAX_CLIENTS, -1);
        if (nEvents == -1)
        {
            close(epollFD);
            throw ServerException("Error: Failed to wait for events");
        }

        for (int i = 0; i < nEvents; i++)
        {
            int fd = events[i].data.fd;

            // Check for new connections
            it = std::find(_masterFDs.begin(), _masterFDs.end(), fd);
            if (it != _masterFDs.end())
            {
                clientFD client = _monitoredFDs[fd]->acceptSocket();
                if (client == -1)
                {
                    std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
                    continue;
                }
                if (setNonBlocking(client) == -1)
                {
                    std::cerr << "Error setting client socket to non-blocking mode: " << strerror(errno) << std::endl;
                    close(client);
                    continue;
                }

                _clients[client] = new Event(client, _config);
                std::cout << "Accepted connection from " << inet_ntoa(_monitoredFDs[fd]->getAddressInfo().sin_addr)
                        << " on port " << ntohs(_monitoredFDs[fd]->getAddressInfo().sin_port) << std::endl; //! DELETE
                std::cout << "Added client FD: " << client << " to _clients" << std::endl; //! DELETE
                    
                // Add cliet socket to epoll for monitoring (both read and write)
                struct epoll_event clientEvent;
                clientEvent.data.fd = client;
                clientEvent.events = EPOLLIN | EPOLLET | EPOLLONESHOT;

                if (epoll_ctl(epollFD, EPOLL_CTL_ADD, client, &clientEvent) == -1)
                {
                    std::cerr << "Error adding client socket to epoll instance: " << strerror(errno) << std::endl;
                    delete _clients[client];
                    _clients.erase(client);
                    close(client);
                }

            }
            else
            {
                // Handle client events

                // Check if client is a read or write event
                uint32_t eventFlags = events[i].events;

                if (_clients.find(fd) == _clients.end())
                {
                    std::cerr << "Error: Client FD not found in _clients: " << fd << std::endl;
                    continue;
                }

                try
                {
                    // Handle read
                    if (eventFlags & EPOLLIN)
                    {
                        std::cout << "EPOLLIN triggered for FD: " << fd << std::endl; //! DELETE
                        _clients[fd]->handleEvent(EPOLLIN, &_log);

                        // after event processing is complete, modify the epoll to monitor EPOLLOUT
                        struct epoll_event clientEvent;
                        clientEvent.data.fd = fd;
                        clientEvent.events = EPOLLOUT | EPOLLET | EPOLLONESHOT;

                        if (epoll_ctl(epollFD, EPOLL_CTL_MOD, fd, &clientEvent) == -1)
                            throw ServerException("Error: Failed to modify epoll instance to monitor EPOLLOUT");
                    }
                    //Handle write event
                    if (eventFlags & EPOLLOUT)
                    {
                        std::cout << "EPOLLOUT triggered for FD: " << fd << std::endl; //! DELETE
                        _clients[fd]->handleEvent(EPOLLOUT, &_log);

                        if (_clients[fd]->getResponseKeepAlive().empty())
                        {
                            std::cerr << "Error: Response header is empty" << std::endl;
                            delete _clients[fd];
                            _clients.erase(fd);
                            close(fd);
                            std::cout << "Closed connection for client FD: " << fd << std::endl; //! DELETE
                        }
                        else
                        {
                            // after event processing is complete, modify the epoll to monitor EPOLLIN
                            struct epoll_event clientEvent;
                            clientEvent.data.fd = fd;
                            clientEvent.events = EPOLLIN | EPOLLET | EPOLLONESHOT;

                            if (epoll_ctl(epollFD, EPOLL_CTL_MOD, fd, &clientEvent) == -1)
                            {
                                std::cerr << "Error rearming FD " << fd << ": " << strerror(errno) << std::endl; //! DELETE
                                delete _clients[fd];
                                _clients.erase(fd);
                                close(fd);
                            }
                        }
                    }
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << std::endl;
                    delete _clients[fd]; 
                    _clients.erase(fd);
                    _monitoredFDs.erase(fd);
                    close(fd);
                }
                
            }
        }
    }
    close(epollFD);
}
