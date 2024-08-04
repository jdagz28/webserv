/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 01:13:31 by jdagoy            #+#    #+#             */
/*   Updated: 2024/08/04 01:19:08 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "Config.hpp"
#include "HttpRequest.hpp"


class HttpResponse
{
    private:
        HttpRequest     &_request;
        Config          &_config;
        int             _status;
        int             _error;
        std::string     _errorMsg;
        int             _client_socket;
        
        
        HttpResponse(const HttpResponse &copy);
        HttpResponse &operator=(const HttpResponse &copy);

    public:
        HttpResponse(const HttpRequest &request,
                        const Config &config,
                        int client_socket);
        ~HttpResponse();


        void    generateResponse();
        void    generateResponseLine();
        void    generateResponseHeaders();
        void    generateResponseBody();
        
    
        
}