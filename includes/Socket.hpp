/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagz28 <jdagz28@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 02:41:19 by jdagz28           #+#    #+#             */
/*   Updated: 2025/01/14 21:07:01 by jdagz28          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <vector>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <exception>
#include "webserv.hpp"

#define SOCKET_MAXCONNECIONS 1024

class Socket
{
    private:
        int                             _socketStatus;  
        int                             _socketFD;
        struct sockaddr_in              _addressInfo;
        std::string                     _ip;
        int                             _port;


        Socket();

        void    initAddressInfo();

    public:
        Socket(const std::string &ip, int port);
        Socket(const Socket &copy);
        Socket &operator=(const Socket &copy);
        ~Socket();

        void    initSocket();
        void    createSocket();
        void    bindSocket();
        void    listenSocket();
        int     acceptSocket();

        int     getSocketFD() const;
        struct sockaddr_in getAddressInfo() const;

        class SocketException : public std::exception
        {
            private:
                std::string     _exceptMsg;
            public:
                SocketException(const std::string &msg)
                    : _exceptMsg(msg) {};
                ~SocketException() throw() {};
                const char *what() const throw();
        };
};


#endif