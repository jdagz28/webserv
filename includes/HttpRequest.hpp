/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 02:11:42 by jdagoy            #+#    #+#             */
/*   Updated: 2024/07/30 23:28:18 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPRREQUEST_HPP

#include <map>
#include <vector>

class HttpRequest
{
    private:
        std::string                             _method;
        std::string                             _uri;
        std::string                             _version;
        std::map<std::string, std::string>      _headers;
        std::string                             _body;
        int                                     _status;

        parseHttpRequest();

    public:
        HttpRequest();
        HttpRequest(const HttpRequest &copy);
        HttpRequest &operator=(const HttpRequest &copy);
        ~HttpRequest();

}


#endif
