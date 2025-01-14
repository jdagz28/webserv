/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagz28 <jdagz28@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 13:30:47 by jdagz28           #+#    #+#             */
/*   Updated: 2025/01/14 14:35:29 by jdagz28          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENT_HPP
#define EVENT_HPP

#include "Config.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "webserv.hpp"


class Event
{
    private:
        clientFD        _fd;
        Config          _config;
        HttpRequest*    _request;
        HttpResponse*   _response;
        

    public:
        Event(clientFD fd, const Config &config);
        ~Event();

        void    handleEvent(uint32_t events);
};


#endif