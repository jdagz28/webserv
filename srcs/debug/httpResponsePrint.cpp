/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpResponsePrint.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 11:31:41 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/04 11:33:17 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include <iostream>
#include <string>


void    printHttpResponse(const std::string &response)
{
    std::cout << "===== RESPONSE =====" << std::endl;
    std::cout << response << std::endl;
    std::cout << "====================" << std::endl;
}
