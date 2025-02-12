/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>            +#+  +:+       +#+        */
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

int Event::s_eventCount = 0;

Event::Event(clientFD fd, int epollFD, const Config &config)
    : _fd(fd), _epollFD(epollFD), _config(config), _request(NULL), _response(NULL), _finished(false)
{
	++s_eventCount;
}

Event::~Event()
{
    delete _request;
    if (_response)
    {
        delete _response;
        _response = NULL;
    }
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
    try
	{
		if (_fd < 0) 
			throw std::runtime_error("Invalid file descriptor in Event::handleEvent"); 

		if (events & EPOLLIN)
		{
			if (!_request)
				_request = new HttpRequest(_fd);
			_request->requestToBuffer();
			if (!_request->isHeadersComplete())
				return ;
			
			size_t expected = _request->expectedTotalBytes();
			if (_request->getBufferSize() < expected)
				return ;

			if (_request->getStatusCode() == OK)
				_request->parseHttpRequest();
						
			if (!_request->getRequestLine().getUri().empty() && checkServerName())
			{
				log->request(*_request);
				(void)log;
				_response = new HttpResponse(*_request, _config, _fd);

				struct epoll_event ev;
				ev.data.fd = _fd;
				ev.events = EPOLLOUT;
				if (epoll_ctl(_epollFD, EPOLL_CTL_MOD, _fd, &ev) == -1)
				{
					perror("epoll_ctl: modify to EPOLLOUT");
					close(_fd);
					_finished = true;
					return;
				}
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
				{
					_finished = false;
					_request->reset();
					return ;
				}
			}
			_finished = true;
		}

		if (events & (EPOLLERR | EPOLLHUP))
		{
			if (_response)
			{
				delete _response;
				_response = NULL;
			}
			close(_fd);
			_finished = true;
		}	
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		if (_response) 
		{
			delete _response;
			_response = NULL;
		}
		if (_fd >= 0) 
		{
			close(_fd);
			_fd = -1;
		}
		_finished = true;
	}
	
}

std::string Event::getResponseKeepAlive() const
{
    std::string response = _response->getHeader("Connection");

    return (response);
}

bool Event::isFinished() const
{
	return (_finished);
}
