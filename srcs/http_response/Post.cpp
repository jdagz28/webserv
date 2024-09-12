/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Post.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 10:57:50 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/12 22:12:51 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "webserv.hpp"
#include "ServerConfig.hpp"
#include "HttpRequestLine.hpp"
#include "LocationConfig.hpp"
#include <string>

/**
 *  STATIC
 *  - check Loc if POST method is allowed
 *  - check if Form
 *  - check if supported media
 */


/**
 *  POST Media 
 *  - form data (multi-part, single form(search and redirect to Google))
 *  - image upload (jpeg, jpg, png, gif)
 */

void    HttpResponse::processRequestPOST()
{
    if (!checkLocConfigAndRequest())
        return ;
    if (!_request.isSupportedMediaPOST())
    {
        setStatusCode(UNSUPPORTED_MEDIA_TYPE);
        return ;
    }
}