/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 01:19:13 by jdagoy            #+#    #+#             */
/*   Updated: 2024/08/04 01:30:38 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.cpp"

HttpResponse::HttpResponse(const HttpRequest &rquest,
                            const Config &config,
                            int client_socket)
    : _request(request), _config(config), _status(0), _error(0), _errorMsg(""), _client_socket(client_socket)
{
}