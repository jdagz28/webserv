/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 02:24:09 by jdagoy           #+#    #+#             */
/*   Updated: 2025/01/15 13:55:28 by jdagoy          ###   ########.fr       */
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
    _eventsQueue = epoll_create1(0);
    if (_eventsQueue == -1)
        throw ServerException("Error: Failed to create epoll instance");
}

Server::~Server()
{
    clearClients();
    clearSockets();
    close(_eventsQueue);
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
    
    try
    {
        createSockets();
        setSignals();
    }
    catch (const std::exception& e)
    {
        _serverStatus = -1;
        std::cerr << e.what() << std::endl;
    }
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
            if (setNonBlocking(socket->getSocketFD()) == -1)
                throw ServerException("Error: Failed to set master socket to non-blocking mode");
            socket->bindSocket();
            socket->listenSocket();

            _monitoredFDs[socket->getSocketFD()] = socket;
            _masterFDs.push_back(socket->getSocketFD());
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

const char* Server::ServerException::what() const throw()
{
    return (_exceptMsg.c_str());
}

void    Server::checkForNewConnections(clientFD newClient)
{    
    if (newClient == -1)
        throw ServerException("Error: Failed to accept connection");
    if (setNonBlocking(newClient) == -1)
        throw ServerException("Error: Failed to set client socket to non-blocking mode");
                    
    _clients[newClient] = new Event(newClient, _config);
}

void    Server::addToEpoll(int epollFD, int fd, uint32_t events)
{
    struct epoll_event event;
    bzero(&event, sizeof(event));
    event.data.fd = fd;
    event.events = events;

    if (epoll_ctl(epollFD, EPOLL_CTL_ADD, fd, &event) == -1)
    {
        close(epollFD);
        throw ServerException("Error: Failed to add fd to epoll instance");
    }
}

void    Server::handleEvent(clientFD fd, uint32_t eventFlags)
{
    if (_monitoredFDs.find(fd) != _monitoredFDs.end())
    { 
        clientFD newClient = _monitoredFDs[fd]->acceptSocket();

        checkForNewConnections(newClient);                
        addToEpoll(_eventsQueue, newClient, EPOLLIN);
        // _log.acceptedConnection(_monitoredFDs[fd]->getAddressInfo(), ntohs(_monitoredFDs[fd]->getAddressInfo().sin_port));
    }
    else if (_clients.find(fd) != _clients.end())
        _clients[fd]->handleEvent(eventFlags, &_log);
    else
        throw ServerException("Error: FD not found in _monitoredFDs or _clients");
}


void    Server::runServer()
{
    if (_serverStatus == -1)
        return;

    try
    {
        std::vector<socketFD>::iterator it;
        for (it = _masterFDs.begin(); it != _masterFDs.end(); it++) 
            addToEpoll(_eventsQueue, *it, EPOLLIN);
        
        while (true)
        {
            int nEvents = epoll_wait(_eventsQueue, _eventsList, MAX_CLIENTS, -1);
            if (nEvents == -1)
            {
                if (errno == EINTR)
                    continue;;
                throw ServerException("Error: Failed queueing for events");
            }
            
            for (int i = 0; i < nEvents; i++)
            {
                clientFD fd = _eventsList[i].data.fd;
                uint32_t eventFlags = _eventsList[i].events;

                handleEvent(fd, eventFlags);
            }
        }
    }
    catch (const std::exception& e)
    {
        _serverStatus = -1;
        std::cerr << e.what() << std::endl;
    }
}

int Server::setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return (-1);
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
        return (-1);
        
    return (0);
}
