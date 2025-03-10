/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 02:24:09 by jdagoy           #+#    #+#             */
/*   Updated: 2025/01/15 13:55:28 by jdagoy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "webserv.hpp"
#include <iostream>
#include <algorithm>
#include <arpa/inet.h> 
#include <fcntl.h>
#include <sys/epoll.h>
#include <cstring>


Server::Server(const Config &config) 
    : _serverStatus(0), _config(config), _masterFDs(), _monitoredFDs(), _clients(), _log()
{
	validateConfigPorts();	
    _log.checkConfig(config);
    _eventsQueue = epoll_create(1);
    if (_eventsQueue == -1)
        throw ServerException("Error: Failed to create epoll instance");
}

Server::~Server()
{
    clearSockets();
	clearClients();
	if (_eventsQueue >= 0)
	{
		if (close(_eventsQueue) == -1)
			throw ServerException("Error: Failed to close epoll instance");
	}
}

void   Server::clearClients()
{
    std::map<clientFD, Event *>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); it++)
	{
		delete it->second;
	}
    _clients.clear();
}

void    Server::clearSockets()
{
    std::map<socketFD, Socket *>::iterator it;
    for (it = _monitoredFDs.begin(); it != _monitoredFDs.end(); it++)
	{
        delete it->second;
	}
	_monitoredFDs.clear();
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
		std::vector <int> ports = it->getPort();
		std::vector<int>::iterator itPort;
		for (itPort = ports.begin(); itPort != ports.end(); itPort++)
		{
			try
			{
				Socket *socket = new Socket(it->getIP(), *itPort);
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
}

void    Server::signalHandler(int signum)
{
	if (signum == SIGINT)
		std::cout << " - CTRL + C detected" << std::endl;	 
	std::cout << "==========================================" << std::endl;
    std::cout << "               SHUTTING DOWN              " << std::endl; 
	std::cout << "==========================================" << std::endl;
	g_running = 0;
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
	{
        throw ServerException("Error: Failed to accept connection");
	}
	if (setNonBlocking(newClient) == -1)
        throw ServerException("Error: Failed to set client socket to non-blocking mode");
	std::map<clientFD, Event *>::iterator it = _clients.find(newClient);
	if (it != _clients.end())
		return ;
    _clients[newClient] = new Event(newClient, _eventsQueue, _config);
}

void    Server::addToEpoll(int epollFD, int fd, uint32_t events)
{
    struct epoll_event event;
    std::memset(&event, 0, sizeof(event));
    event.data.fd = fd;
    event.events = events;

    if (epoll_ctl(epollFD, EPOLL_CTL_ADD, fd, &event) == -1)
    {
        if (close(epollFD) == -1)
			throw ServerException("Error: Failed to close fd that failed to add to epoll instance");
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
    }
    else if (_clients.find(fd) != _clients.end())
        _clients[fd]->handleEvent(eventFlags, &_log);
}

void	Server::cleanupFinishedEvents() 
{
	std::map<clientFD, Event *>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); )
	{
		if (it->second->isFinished())
		{
			epoll_ctl(_eventsQueue, EPOLL_CTL_DEL, it->first, NULL);
            delete it->second;
			_clients.erase(it++);
		}
		else
		{
			++it;
		}
	}
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
        
        while (g_running)
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
			cleanupFinishedEvents();
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


void	Server::validateConfigPorts()
{
	std::vector<ServerConfig> servers = _config.getServerConfig();
	std::vector<ServerConfig>::iterator it;
	std::vector<int> ports;
	std::vector<int>::iterator itPort;
	std::vector<int> allPorts;

	for (it = servers.begin(); it != servers.end(); it++)
	{
		ports = it->getPort();
		for (itPort = ports.begin(); itPort != ports.end(); itPort++)
		{
			if (std::find(allPorts.begin(), allPorts.end(), *itPort) != allPorts.end())
				throw ServerException("Error: Duplicate port number in configuration file");
			allPorts.push_back(*itPort);
		}
	}
}