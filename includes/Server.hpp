/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 02:24:21 by jdagoy           #+#    #+#             */
/*   Updated: 2025/01/15 13:37:36 by jdagoy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <vector>
#include <csignal>
#include <exception>
#include <cstdlib>
#include <sys/epoll.h>  
#include "Config.hpp"
#include "webserv.hpp"
#include "Socket.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Logger.hpp"
#include "Event.hpp"

class Server 
{
    private:
        int                             _serverStatus;
        Config                          _config;
        std::vector<socketFD>           _masterFDs;
        std::map<socketFD, Socket *>    _monitoredFDs;
        std::map<clientFD, Event *>     _clients;
        Logger                          _log;
        int                             _eventsQueue;
        epoll_event                     _eventsList[MAX_CLIENTS];
        
        Server();
        Server(const Server &copy);
        Server &operator=(const Server &copy);
        
        void    createSockets();
        void    setSignals();
        void    clearSockets();
        void    clearClients();
        int setNonBlocking(int fd);

        void    checkForNewConnections(clientFD newClient);
        void    addToEpoll(int epollFD, int fd, uint32_t events);
        void    handleEvent(clientFD fd, uint32_t eventFlags);


    public:
        Server(const Config &config);
        ~Server();

        void    initServer();
        void    runServer();
        void    closeServer();

        static void    signalHandler(int signum);

        class ServerException : public std::exception
        {
            private:
                std::string     _exceptMsg;
            public:
                ServerException(const std::string &msg)
                    : _exceptMsg(msg) {};
                ~ServerException() throw() {};
                const char *what() const throw();
        };

};


#endif