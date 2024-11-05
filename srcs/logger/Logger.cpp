/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 10:15:02 by jdagoy            #+#    #+#             */
/*   Updated: 2024/11/05 10:39:26 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

void Logger::checkConfig(const Config &config)
{
    std::vector<int> ports = config.getPortsToServe();
    std::vector<int>::const_iterator it;
    for (it = _ports.begin(); it != _ports.end(); it++)
        listening()
}