/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagz28 <jdagz28@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 02:24:21 by jdagz28           #+#    #+#             */
/*   Updated: 2025/01/07 02:53:48 by jdagz28          ###   ########.fr       */
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
        Config                          _config;
        std::map<socketFD, Socket *>    _sockets;
        
        Server();
        Server(const Server &copy);
        Server &operator=(const Server &copy);
        

    public:
        Server(const Config &config);
        ~Server();

};


#endif