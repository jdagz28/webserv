/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagz28 <jdagz28@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 13:36:17 by jdagz28           #+#    #+#             */
/*   Updated: 2025/01/14 22:45:01 by jdagz28          ###   ########.fr       */
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
    try
    {
        if (_fd < 0) 
            throw std::runtime_error("Invalid file descriptor in Event::handleEvent"); //!CHANGE

        if (events & EPOLLIN)
        {
            _request = new HttpRequest(_fd);
            log->request(*_request);
            _response = new HttpResponse(*_request, _config, _fd);
            _response->sendResponse();
            log->response(*_response);
            // printHttpResponse(_response->getHttpResponse());
        }

        if (events & EPOLLOUT)
        {
            if (_response)
                _response->sendResponse();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    
}

std::string Event::getResponseKeepAlive()
{
    std::string response = _response->getHeader("Keep-Alive");

    return (response);
}