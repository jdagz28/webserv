/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 23:18:08 by jdagoy            #+#    #+#             */
/*   Updated: 2025/02/22 21:04:34 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
#include "webserv.hpp"


Cgi::Cgi()
{}

Cgi::Cgi(const std::string &path, const std::map<std::string, std::string> &env)
	: _path(path), _env(env)
{}

Cgi::~Cgi()
{}

void 	Cgi::setScriptPath(const std::string &path)
{
	_path = path;
}

void 	Cgi::setEnv(const std::map<std::string, std::string> &env)
{
	_env = env;
}