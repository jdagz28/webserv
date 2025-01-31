/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setCookies.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 04:53:47 by jdagoy            #+#    #+#             */
/*   Updated: 2025/01/31 13:23:01 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"

/** ========================================================================== 
* set-cookie-header = "Set-Cookie:" SP set-cookie-string
* set-cookie-string = cookie-pair *( ";" SP cookie-av )
* cookie-pair       = cookie-name "=" cookie-value
* cookie-name       = token
* cookie-value      = *cookie-octet / ( DQUOTE *cookie-octet DQUOTE )
* cookie-octet      = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E
*                    ; US-ASCII characters excluding CTLs,
*                    ; whitespace DQUOTE, comma, semicolon,
*                    ; and backslash
* token             = <token, defined in [RFC2616], Section 2.2>
* cookie-av         = expires-av / max-age-av / domain-av /
*                     path-av / secure-av / httponly-av /
*       s              extension-av
* expires-av        = "Expires=" sane-cookie-date
* sane-cookie-date  = <rfc1123-date, defined in [RFC2616], Section 3.3.1>
* max-age-av        = "Max-Age=" non-zero-digit *DIGIT
*                     ; In practice, both expires-av and max-age-av
*                     ; are limited to dates representable by the
*                     ; user agent.
* secure-av         = "Secure"
* httponly-av       = "HttpOnly"
==============================================================================*/

void    HttpResponse::setCookies(std::string key, std::string value)
{
    std::string cookiesStr;
    
    if (_headers["Set-Cookie"].empty())

        cookiesStr = key + "=" + value;
    else
    {
        cookiesStr = _headers["Set-Cookie"];
        cookiesStr += "; " + key + "=" + value;
    }
    if (!cookiesStr.empty())
        _headers["Set-Cookie"] = cookiesStr;
    //! Add expiry
    //! max-age
}


void    HttpResponse::setLanguage()
{
    if (_request.getFormData("language") == "en")
        _headers["Location"] = "html/language_en.html";
    else if (_request.getFormData("language") == "fr")
        _headers["Location"] = "html/language_fr.html";
    setCookies("language", _request.getFormData("language"));
    setStatusCode(SEE_OTHER);
}