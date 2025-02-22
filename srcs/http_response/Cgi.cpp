/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 23:18:08 by jdagoy            #+#    #+#             */
/*   Updated: 2025/02/22 21:31:50 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
#include "webserv.hpp"
#include <iostream>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>

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

void	Cgi::executeCGI()
{
	if (!isValidScript())
		return ;
	std::cout << "Executing CGI script" << std::endl;
}

bool	Cgi::isValidScript()
{
	struct stat 	statbuf;

	if (stat(_path.c_str(), &statbuf) == -1)
	{
		std::cerr << "Error: " << strerror(errno) << std::endl;
		return (false);
	}
	if (!(statbuf.st_mode & S_IXUSR))
	{
		std::cerr << "Error: Script is not executable" << std::endl;
		return (false);
	}
	return (true);
}