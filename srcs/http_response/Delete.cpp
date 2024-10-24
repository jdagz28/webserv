/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Delete.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 21:37:25 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/25 00:29:54 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "webserv.hpp"
#include <string>
#include <map>

void HttpResponse::processRequestDELETE()
{
    std::map<std::string, std::string> toDelete = _request.getFormData();
    
    std::map<std::string, std::string>::iterator it;
    for (it = toDelete.begin(); it != toDelete.end(); it++)
    {
        if (it->first.find("files") == std::string::npos)
        {
            setStatusCode(BAD_REQUEST);
            return ;
        }
        std::string file = it->second;
        std::string path = resolvePath(_serverConfig);
        if (path.empty())
        {
            setStatusCode(NOT_FOUND);
            return ;
        }
        std::string indexPath = buildResourcePath(path, file);
        if (fileExists(indexPath))
        {
            if (remove(indexPath.c_str()) != 0)
                setStatusCode(INTERNAL_SERVER_ERROR);
        }
        else
        {    
            setStatusCode(NOT_FOUND);
            return ;
        }
    }
    setStatusCode(SEE_OTHER);
    _headers["Location"] = "/directory/uploads";
}
