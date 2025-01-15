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
#include <iostream>
#include <sys/epoll.h>

Event::Event(clientFD fd, const Config &config)
    : _fd(fd), _config(config), _request(NULL), _response(NULL)
{
    std::cout << "Event created" << std::endl; //! DELETE
}

Event::~Event()
{
    if (_request)
        delete _request;
    if (_response)
        delete _response;
}

void    Event::handleEvent(uint32_t events, Logger *log)
{
    if (_fd < 0) 
        throw std::runtime_error("Invalid file descriptor in Event::handleEvent"); //!CHANGE

    if (events & EPOLLIN)
    {
        std::cout << "EPOLLIN" << std::endl; //! DELETE
        _request = new HttpRequest(_fd);
                    
        if (!_request->getRequestLine().getUri().empty())
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
        std::cout << "EPOLLOUT" << std::endl; //! DELETE
        if (_response)
        {
            _response->sendResponse();
            log->response(*_response);
            delete _response;
            _response = NULL;

            if (!_request->isConnectionClosed())
            {
                std::cout << "Keep-Alive: keeping connection alive for fd" << _fd <<  std::endl; //! DELETE
                return ;
            }
        }
        close(_fd);
        delete this;
    }

    if (events & (EPOLLERR | EPOLLHUP))
    {
        std::cout << "EPOLLERR" << std::endl; //! DELETE
        close(_fd);
        delete this;
    }
}

std::string Event::getResponseKeepAlive()
{
    std::string response = _response->getHeader("Keep-Alive");

    return (response);
}