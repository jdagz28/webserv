/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagz28 <jdagz28@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 02:24:21 by jdagz28           #+#    #+#             */
/*   Updated: 2025/01/07 14:14:38 by jdagz28          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <vector>
#include "Config.hpp"
#include "webserv.hpp"
#include "Socket.hpp"

typedef int socketFD;
typedef int clientFD;

class Server 
{
    private:
        int                             _serverStatus;
        const Config                    &_config;
        std::map<socketFD, Socket *>    _sockets;
        std::map<clientFD, Socket *>    _clients;
        
        Server();
        Server(const Server &copy);
        Server &operator=(const Server &copy);
        
        void    create_sockets();

    public:
        Server(const Config &config);
        ~Server();

        void    initServer();
        void    runServer();
        void    closeServer();

};


#endif