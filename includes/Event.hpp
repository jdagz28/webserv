/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 13:30:47 by jdagoy           #+#    #+#             */
/*   Updated: 2025/01/15 14:06:20 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENT_HPP
#define EVENT_HPP

#include "Config.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "webserv.hpp"
#include "Logger.hpp"

class Event
{
    private:
        clientFD        _fd;
		int				_epollFD;
        const Config    &_config;
        HttpRequest*    _request;
        HttpResponse*   _response;
		bool			_finished;
		static int 		s_eventCount;

		Event(const Event &copy);
		Event &operator=(const Event &copy);
        bool    checkServerName();
       
    public:
        Event(clientFD fd, int epollFD, const Config &config);
        ~Event();

        void    handleEvent(uint32_t events, Logger *log);
        std::string	getResponseKeepAlive() const;
		bool	isFinished() const;
};

#endif