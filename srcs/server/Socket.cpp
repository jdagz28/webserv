/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagz28 <jdagz28@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 02:56:15 by jdagz28           #+#    #+#             */
/*   Updated: 2025/01/07 15:51:17 by jdagz28          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include <iostream>
#include <cstring>
#include <arpa/inet.h> 


Socket::Socket(const std::string &ip, int port)
    : _socketStatus(0), _socketFD(-1), _ip(ip), _port(port), _addressInfo()
{
}

Socket::~Socket()
{
    if (_socketFD != -1)
    {
        if(close(_socketFD) == -1)
        {
            _socketStatus = -1;
            std::cerr << "Error: failed to close socket" << std::endl;
            throw std::exception();
        }
        _socketFD = -1;
    }
}

void    Socket::initSocket()
{
    initAddressInfo();
    _socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (_socketFD == -1)
    {
        _socketStatus = -1;
        std::cerr << "Error: failed to create socket" << std::endl;
        throw std::exception();
    }
}

void    Socket::createSocket()
{
    initSocket();
    if (_socketFD == -1)
        throw std::exception();
    
    bindSocket();
;
}


void    Socket::initAddressInfo()
{
    struct in_addr addr;
    if (inet_pton(AF_INET, _ip.c_str(), &addr) <= 0) {
        std::cerr << "Error: Invalid IP address provided: " << _ip << std::endl;
        throw std::invalid_argument("Invalid IP address: " + _ip);
    }
    
    memset(&_addressInfo, 0, sizeof(_addressInfo));
    _addressInfo.sin_family = AF_INET;
    _addressInfo.sin_port = htons(_port);
    _addressInfo.sin_addr.s_addr = INADDR_ANY;

    std::cout << "Initialized address info: IP = " << _ip << ", Port = " << _port << std::endl; //! DELETE
}

void    Socket::bindSocket()
{
    int opt = 1;
    if (setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        _socketStatus = -1;
        std::cerr << "Error: setsockopt failed" << std::endl;
        throw std::exception();
    }

    if (bind(_socketFD, (struct sockaddr*)&_addressInfo, sizeof(_addressInfo)) == -1)
    {
        _socketStatus = -1;
        std::cerr << "Error: failed to bind socket" << std::endl;
        throw std::exception();
    }
    std::cout << "Socket successfully bound to " << _ip << ":" << _port << std::endl; //! DELETE
}

void    Socket::listenSocket()
{
    if (listen(_socketFD, SOCKET_MAXCONNECIONS) == -1)
    {
        _socketStatus = -1;
        std::cerr << "Error: Could not listen to socket." << std::endl;
        throw std::exception();
    }
}

void    Socket::acceptSocket()
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_socket = accept(_socketFD, (struct sockaddr*)&client_addr, &client_len);
    if (client_socket == -1)
    {
        _socketStatus = -1;
        std::cerr << "Error: Failed to accept connection" << std::endl;
        throw std::exception();
    }
}

int     Socket::getSocketFD() const
{
    return _socketFD;
}