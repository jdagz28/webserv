/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/07 01:22:00 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/04 11:33:32 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEBUG_HPP
#define DEBUG_HPP

class HttpRequest;
class Config;
class HttpResponse;

void    printConfigData(const Config &config);
void    printHttpRequest(const HttpRequest &request);
void    printHttpResponse(const std::string &response);



#endif
