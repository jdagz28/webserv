/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagz28 <jdagz28@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 02:24:21 by jdagz28           #+#    #+#             */
/*   Updated: 2025/01/10 15:44:17 by jdagz28          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <vector>
#include <csignal>
#include <exception>
#include <cstdlib>
#include "Config.hpp"
#include "webserv.hpp"
#include "Socket.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Logger.hpp"

typedef int socketFD;
typedef int clientFD;
typedef int FD;

class Server 
{
    private:
        int                             _serverStatus;
        Config                          _config;
        std::vector<socketFD>           _masterFDs;
        std::map<socketFD, Socket *>    _monitoredFDs;
        // std::map<clientFD, Socket *>    _clients; //! better with clientFd and Event struct
        Logger                          _log;
        fd_set                          _readFDs;
        
        Server();
        Server(const Server &copy);
        Server &operator=(const Server &copy);
        
        void    createSockets();
        // void    setSignals();
        void    clearSockets();
        void    handleConnections();

        void    addMasterFD();
        void    reInitMonitoredFDs();
        int getMaxFD();

    public:
        Server(const Config &config);
        ~Server();

        void    initServer();
        void    runServer();
        void    closeServer();

        void    signalHandler(int signum);

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