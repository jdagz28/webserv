/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookies.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 08:50:11 by jdagoy            #+#    #+#             */
/*   Updated: 2024/12/06 12:56:54 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include "webserv.hpp"
#include <iterator>

const std::string HttpRequest::printCookies() const
{
    std::string cookies;
    if (_cookies.empty())
        return (std::string()); 
    else
    {        
        std::map<std::string, std::string>::const_iterator it;
        std::map<std::string, std::string>::const_iterator lastEntry;
        lastEntry = --_cookies.end();
        for (it = _cookies.begin(); it != _cookies.end(); it++)
        {
            cookies += it->first + "=" + it->second;
            if (it != lastEntry)
                cookies += "; ";
        }
    }
    return (cookies);
}

const std::map<std::string, std::string> &HttpRequest::getCookies() const
{
    return (_cookies);
}