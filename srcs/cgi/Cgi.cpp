/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 19:52:54 by romvan-d          #+#    #+#             */
/*   Updated: 2025/03/06 10:20:38 by jdagoy           ###   ########.fr       */
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
#include <stdio.h>

# define BUFFERSIZE 2048
static void freeTab(char **tab) {
    size_t i = 0;

    while (tab[i] != NULL) {
        delete[] tab[i];
        i++;
    }
    delete[] tab;
}

Cgi::Cgi ()
{

}

Cgi::Cgi(const HttpRequestLine & requestLine, const HttpRequest & request, const std::string path, const std::string &uploadDir)
{
	this->path = path;
	this->args.push_back("");
	this->env["REQUEST_METHOD="] = requestLine.getMethod();
	this->env["UPLOAD_DIR="] = uploadDir;
	std::string uri = requestLine.getUri();
	size_t querryPos = uri.find("?");
	
	if (requestLine.getMethod() == "GET")
	{
		this->whichMethod = 0;
		if (querryPos != std::string::npos)
			this->data = uri.substr(querryPos + 1);//need the query string;
		this->env["CONTENT_LENGTH="] = "NULL";
		this->env["QUERRY_STRING="] = this->data;
	}
	else if (requestLine.getMethod() == "POST")
	{
		this->whichMethod = 1;
		this->data = //need the body + '\0';
		this->env["CONTENT_LENGTH="] = //"POST BODY SIZE"
		this->env["QUERRY_STRING="] = "NULL";
	}

	// std::map<std::string, std::string> requestHeaderInfo = request.getHeader();
	this->env["CONTENT_TYPE="] = request.getHeader("content-type"); //requestHeaderInfo["Content-Type"]; 
    this->env["HTTP_ACCEPT="] = request.getHeader("accept"); //requestHeaderInfo["Accept"];
    this->env["HTTP_ACCEPT_ENCODING="] = request.getHeader("accept-encoding"); //requestHeaderInfo["Accept-Encoding"];
    this->env["HTTP_ACCEPT_LANGUAGE="] = request.getHeader("accept-language"); //requestHeaderInfo["Accept-Language"];
    this->env["PATH_INFO="] = this->path;
    this->env["SERVER_PROTOCOL="] = "HTTP/1.1";
    this->env["HOST="] = request.getHeader("host"); //requestHeaderInfo["Host"];
    this->env["HTTP_USER_AGENT="] = request.getHeader("user-agent"); //requestHeaderInfo["User-Agent"];
    this->env["HTTP_CONNECTION="] = request.getHeader("connection"); //requestHeaderInfo["Connection"];

	std::map<std::string, std::string>::const_iterator it;
	for (it = this->env.begin(); it != this->env.end(); it++)
	{
		std::cout << it->first << it->second << std::endl;
	}
}

Cgi::Cgi(const Cgi &other) : env(other.env)
{

}

Cgi::~Cgi()
{

}

Cgi & Cgi::operator=(const Cgi & other)
{
	this->env = other.env;
	return (*this);
}

char ** Cgi::convertEnv(std::map<std::string, std::string> env)
{
	size_t size = env.size();
	char ** envtable = new char*[size + 1];

	if (!envtable)
	{
		std::exit(1);
	}
	size_t j = 0;
	for (std::map<std::string, std::string>::iterator i = env.begin(); i != env.end(); ++i)
	{
		std::string str = i->first + i->second;
		envtable[j] = new char[str.length() + 1];
		if (!envtable[j])
		{
			freeTab(envtable);
			std::exit(1);
		}
		strcpy(envtable[j], str.c_str());
		j++;
	}
	envtable[j] = NULL;
	return(envtable);
}

char ** Cgi::convertArgs(std::vector<std::string> args)
{
	size_t size = args.size();
	char ** argstable = new char*[size + 1];
	if (!argstable)
	{
		std::exit(1);
	}
	size_t j = 0;
	for (std::vector<std::string>::iterator i = args.begin(); i != args.end(); ++i)
	{
		argstable[j] = new char[args[j].length() + 1];
		if (!argstable[j])
		{
			freeTab(argstable);
			std::exit(1);
		}
		strcpy(argstable[j], args[j].c_str());
		j++;
	}
	argstable[j] = NULL;
	return (argstable);
}

std::string Cgi::readPipe(int pipeRead)
{
	int readChars;
	std::string output = "";
	char buffer[BUFFERSIZE];
	
	while ((readChars = read(pipeRead, buffer, BUFFERSIZE)))
	{
		if (readChars == -1)
			throw CgiError();
	output.append(buffer,readChars);
	}
	return output;
}

std::string Cgi::runCgi()
{
	int pipeCGI[2];
	pid_t pidCgi;
	std::string cgiOutput;
	if (pipe(pipeCGI) == -1)
	{
		throw CgiError();
	}
	pidCgi = fork();
	if (pidCgi == -1)
	{
		close(pipeCGI[0]);
		close(pipeCGI[1]);
		throw CgiError();
	}
	else if (pidCgi == 0)
	{
		FILE * tmpFileWrite = std::fopen("/tmp/CgiDataUpload", "w");
		if (!tmpFileWrite)
		{
			std::exit(1);
		}
		if (this->whichMethod == POST)
		{
			char const * writingRecipient = this->data.c_str();
			if (write(fileno(tmpFileWrite), writingRecipient, this->data.length()) == -1)
			{
				std::exit(1);
			}
		}
		close(fileno(tmpFileWrite));

		FILE * tmpFileRead = std::fopen("/tmp/CgiDataUpload", "r");
		dup2(fileno(tmpFileRead), STDIN_FILENO);
		close(fileno(tmpFileRead));

		char **args = convertArgs(this->args);
		char **env = convertEnv(this->env);
		size_t lastSlashChar = this->path.find_last_of("/");
		int ret = chdir(this->path.substr(0, lastSlashChar).c_str());
		if (ret == -1)
		{
			freeTab(args);
			freeTab(env);
			std::exit(1);
		}
		ret = execve(this->path.c_str(), args, env);
		if (ret == -1)
		{
			freeTab(args);
			freeTab(env);
			std::exit(1);
		}
	}
	else
	{
		close(pipeCGI[1]);
		int status;
		waitpid(pidCgi, &status, 0);
		if (WIFEXITED(status))
		{
			int exitcode = WEXITSTATUS(status);
			if (exitcode != 0)
			{
				close(pipeCGI[0]);
				throw CgiError();
			}
		}
		cgiOutput = readPipe(pipeCGI[0]);
		close(pipeCGI[0]);
	}
	std::remove("/tmp/CgiDataUpload");
	return cgiOutput;
}

const char *Cgi::CgiError::what() const throw()
{
        return("Cgi::CgiError : Internal  Error Cgi.");
}