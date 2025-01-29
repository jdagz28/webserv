/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 13:36:17 by jdagoy           #+#    #+#             */
/*   Updated: 2025/01/15 14:04:34 by jdagoy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Event.hpp"
#include "debug.hpp"
#include "webserv.hpp"
#include <iostream>
#include <sys/epoll.h>

Event::Event(clientFD fd, const Config &config)
    : _fd(fd), _config(config), _request(NULL), _response(NULL)
{}

Event::~Event()
{
    if (_request)
        delete _request;
    if (_response)
        delete _response;
}

bool    Event::checkServerName()
{
    std::string requestHost = _request->getHost();
    std::string host = requestHost.substr(0, requestHost.find(":"));
    int port = strToInt(requestHost.substr(requestHost.find(":") + 1));

    std::vector<ServerConfig> servers = _config.getServerConfig();
    std::vector<ServerConfig>::const_iterator server;
    for (server = servers.begin(); server != servers.end(); server++)
    {
        if (server->getPort() == port)
        {
            std::vector<std::string> serverNames = server->getServerNames();
            std::vector<std::string>::const_iterator serverName;
            for (serverName = serverNames.begin(); serverName != serverNames.end(); serverName++)
            {
                if (*serverName == host)
                    return (true);
            }
        }
    }
    return (false);
}

void    Event::handleEvent(uint32_t events, Logger *log)
{
    if (_fd < 0) 
        throw std::runtime_error("Invalid file descriptor in Event::handleEvent"); //!CHANGE

    if (events & EPOLLIN)
    {
        _request = new HttpRequest(_fd);
        // printHttpRequest(*_request);
                    
        if (!_request->getRequestLine().getUri().empty() && checkServerName())
        {
            log->request(*_request);
            
            _response = new HttpResponse(*_request, _config, _fd);
            _response->sendResponse();
            log->response(*_response);  
            // printHttpResponse(_response->getHttpResponse());
        }
    }

    if (events & EPOLLOUT)
    {
        if (_response)
        {
            _response->sendResponse();
            log->response(*_response);
            delete _response;
            _response = NULL;

            if (!_request->isConnectionClosed())
                return ;
        }
        close(_fd);
        delete this;
    }

    if (events & (EPOLLERR | EPOLLHUP))
    {
        close(_fd);
        delete this;
    }
}

std::string Event::getResponseKeepAlive()
{
    std::string response = _response->getHeader("Connection");

    return (response);
}