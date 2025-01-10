/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagz28 <jdagz28@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 02:56:15 by jdagz28           #+#    #+#             */
/*   Updated: 2025/01/10 15:51:31 by jdagz28          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include <iostream>
#include <cstring>
#include <arpa/inet.h> 
#include <cerrno>


Socket::Socket(const std::string &ip, int port)
    : _socketStatus(0), _socketFD(-1),  _addressInfo(), _ip(ip), _port(port)
{
    initSocket();
    createSocket();
}

Socket::~Socket()
{
    if (_socketFD != -1)
    {
        if(close(_socketFD) == -1)
        {
            _socketStatus = -1;
            throw SocketException("Error: failed to close socket");
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
        std::cerr << "Error: failed to create socket. Errno: " 
                  << errno << " (" << std::strerror(errno) << ")" << std::endl;
        throw SocketException("Error: failed to create socket");
    }
}

void    Socket::createSocket()
{
    initSocket();
    if (_socketFD == -1)
        throw SocketException("Error: failed to create socket");
    std::cout << "Binding socket to " << _ip << ":" << _port << std::endl; //! DELETE
    bindSocket();
    std::cout << "Socket created and bound to " << _ip << ":" << _port << std::endl; //! DELETE
}


void    Socket::initAddressInfo()
{   
    memset(&_addressInfo, 0, sizeof(_addressInfo));
    _addressInfo.sin_family = AF_INET;
    _addressInfo.sin_port = htons(_port);

    if (_ip.empty())
        _addressInfo.sin_addr.s_addr = INADDR_ANY;
    else
    {
        if (inet_pton(AF_INET, _ip.c_str(), &_addressInfo.sin_addr) <= 0)
        {
            std::string msg = "Error: Invalid IP address provided: " + _ip + "\n";
            throw SocketException(msg);
        }
    }
    
}

void    Socket::bindSocket()
{
    std::cout << "Binding socket to " << _ip << ":" << _port << std::endl; //! DELETE
    
    int opt = 1;
    if (setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        std::cerr << "Error: setsockopt failed. Errno: " 
                  << errno << " (" << std::strerror(errno) << ")" << std::endl; //! DELETE
        _socketStatus = -1;
        throw SocketException("Error: setsockopt failed");
    }

    if (bind(_socketFD, (struct sockaddr*)&_addressInfo, sizeof(_addressInfo)) == -1)
    {
        std::cerr << "Error: failed to bind socket. Errno: " 
                  << errno << " (" << std::strerror(errno) << ")" << std::endl; //! DELETE
        _socketStatus = -1;
        throw SocketException("Error: failed to bind socket");
    }
    std::cout << "Socket successfully bound to " << _ip << ":" << _port << std::endl; //! DELETE
}

void    Socket::listenSocket()
{
    if (listen(_socketFD, SOCKET_MAXCONNECIONS) == -1)
    {
        _socketStatus = -1;
        throw SocketException("Error: Could not listen to socket.");
    }
}

int    Socket::acceptSocket()
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    std::cout << "Debug: Listening socket FD: " << _socketFD << std::endl; //! DELETE
    
    int client_socket = accept(_socketFD, (struct sockaddr*)&client_addr, &client_len);
    if (client_socket == -1)
    {
        std::cerr << "Error: Failed to accept connection. Errno: " 
                  << errno << " (" << std::strerror(errno) << ")" << std::endl;
        _socketStatus = -1;
        throw SocketException("Error: Failed to accept connection");
    }
    std::cout << "Accepted connection from "
              << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) 
              << " on socket FD: " << client_socket << std::endl; //! DELETE

    std::cout << "Accepted connection from "
              << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) 
              << " on socket FD: " << client_socket << std::endl; //! DELETE
    return (client_socket);
}

int     Socket::getSocketFD() const
{
    return _socketFD;
}

struct sockaddr_in Socket::getAddressInfo() const
{
    return _addressInfo;
}

const char *Socket::SocketException::what() const throw()
{
    return _exceptMsg.c_str();
}