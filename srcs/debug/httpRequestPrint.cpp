/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpRequestPrint.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 01:50:05 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/17 02:12:39 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include "debug.hpp"
#include <iostream>


void    printHttpRequest(const HttpRequest &request)
{

    std::cout << "===== REQUEST LINE =====" << std::endl;
    std::cout << "Method: " << request.getRequestLine().getMethod() << std::endl;
    std::cout << "URI: " << request.getRequestLine().getUri() << std::endl;
    std::cout << "Version: " << request.getRequestLine().getVersion() << std::endl;
    std::cout << "========================\n" << std::endl;

    std::cout << "===== HEADERS =====" << std::endl;
    std::map<std::string, std::vector<std::string> >::const_iterator header;
    for(header = request.getHeaders().begin(); header != request.getHeaders().end(); header++)
    {
        std::cout << "\tField Name: " << header->first << std::endl;
        std::vector<std::string>::const_iterator value;
        for (value = header->second.begin(); value != header->second.end(); ++value)
            std::cout << "\t\tField Value: " << *value << std::endl;
        std::cout << "-----" << std::endl;
    }

    if (request.getRequestLine().getMethod() == "POST")
    {
        if (!request.getFormData().empty())
        {
            std::cout << "======= BODY ======" << std::endl;
            std::cout << "===== FORM DATA =====" << std::endl;
            std::map<std::string, std::string>::const_iterator data;
            for (data = request.getFormData().begin(); data != request.getFormData().end(); data++)
            {
                std::cout << "\tField Name: " << data->first << std::endl;
                std::cout << "\tField Value: " << data->second << std::endl;
                std::cout << "-----" << std::endl;
            }
        }
    }
    std::cout << "========================\n" << std::endl;
}