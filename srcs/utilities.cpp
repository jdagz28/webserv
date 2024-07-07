/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilities.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 22:29:37 by jdagoy            #+#    #+#             */
/*   Updated: 2024/07/07 01:44:45 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include <algorithm>

bool isNotSpace(unsigned char ch)
{
    return !std::isspace(ch);
}

void    trimWhitespaces(std::string &str)
{
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), isNotSpace));
    str.erase(std::find_if(str.rbegin(), str.rend(), isNotSpace).base(), str.end());
}

