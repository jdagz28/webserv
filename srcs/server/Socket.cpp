/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagz28 <jdagz28@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 02:56:15 by jdagz28           #+#    #+#             */
/*   Updated: 2025/01/07 03:56:47 by jdagz28          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include <iostream>
#include <cstring>


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