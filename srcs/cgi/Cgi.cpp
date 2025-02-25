/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romvan-d <romvan-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 19:52:54 by romvan-d          #+#    #+#             */
/*   Updated: 2025/02/25 17:09:14 by romvan-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

Cgi::Cgi ()
{

}

Cgi:Cgi(HttpRequestLine & request, std::string path, std::string uploadDir)
{
	this->path = path
	this->args.push_back("")
	this->env["REQUEST_METHOD="] = request.getMethod();
	this->env["UPLOAD_DIR="] = uploadDir;
	
	if (request.getMethod() == "GET")
	{
		this->whichMethod = GET;
		this->data = request.
	}
}

Cgi::Cgi(const Cgi &other) : env(other.env)
{

}

Cgi::~Cgi()
{

}


const char *Cgi::CgiError::what() const throw()
{
        return("Cgi::CgiError : Interal  Error Cgi.");
}