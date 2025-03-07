/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 19:52:54 by romvan-d          #+#    #+#             */
/*   Updated: 2025/03/07 23:32:16 by jdagoy           ###   ########.fr       */
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
#include <sstream>
#include <sys/stat.h>

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

Cgi::Cgi(const HttpRequestLine & requestLine, const HttpRequest & request, const std::string path, const std::string &uploadDir, const std::string &body)
	: status(OK), cgiOutput(""), outputHeaders(), outputBody()
{
	this->path = path;
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
		this->env["QUERY_STRING="] = this->data;
		std::stringstream iss(this->data);
		std::string key;
		std::string value;
		while (std::getline(iss, key, '='))
		{
			std::getline(iss, value, '&');
			this->args.push_back(key + "=" + value);
		}
	}
	else if (requestLine.getMethod() == "POST")
	{
		this->whichMethod = 1;
		this->data = body;
		this->env["CONTENT_LENGTH="] = request.getHeader("content-length");
		this->env["QUERRY_STRING="] = "NULL";
	}

	this->env["CONTENT_TYPE="] = request.getHeader("content-type"); 
    this->env["HTTP_ACCEPT="] = request.getHeader("accept"); 
    this->env["HTTP_ACCEPT_ENCODING="] = request.getHeader("accept-encoding"); 
    this->env["HTTP_ACCEPT_LANGUAGE="] = request.getHeader("accept-language"); 
    this->env["PATH_INFO="] = this->path;
    this->env["SERVER_PROTOCOL="] = "HTTP/1.1";
    this->env["HOST="] = request.getHeader("host"); 
    this->env["HTTP_USER_AGENT="] = request.getHeader("user-agent"); 
    this->env["HTTP_CONNECTION="] = request.getHeader("connection"); 
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
		setStatusCode(INTERNAL_SERVER_ERROR);
		std::exit(1);
	}
	size_t j = 0;
	for (std::map<std::string, std::string>::iterator i = env.begin(); i != env.end(); ++i)
	{
		std::string str = i->first + i->second;
		envtable[j] = new char[str.length() + 1];
		if (!envtable[j])
		{
			setStatusCode(INTERNAL_SERVER_ERROR);
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
		setStatusCode(INTERNAL_SERVER_ERROR);
		std::exit(1);
	}
	size_t j = 0;
	for (std::vector<std::string>::iterator i = args.begin(); i != args.end(); ++i)
	{
		argstable[j] = new char[args[j].length() + 1];
		if (!argstable[j])
		{
			setStatusCode(INTERNAL_SERVER_ERROR);
			freeTab(argstable);
			std::exit(1);
		}
		strcpy(argstable[j], args[j].c_str());
		j++;
	}
	argstable[j] = NULL;
	return (argstable);
}

void Cgi::readPipe(int pipeRead)
{
	int readChars;
	char buffer[BUFFERSIZE];
	
	while ((readChars = read(pipeRead, buffer, BUFFERSIZE)))
	{
		if (readChars == -1)
		{
			setStatusCode(INTERNAL_SERVER_ERROR);
			throw CgiError();
		}
		cgiOutput.append(buffer,readChars);
	}
	std::cout << "CGI OUTPUT\n" << cgiOutput << std::endl;
}

void Cgi::runCgi()
{
	if (!isValidScript())
		throw CgiError();
	
	int pipeCGI[2];
	pid_t pidCgi;
	
	if (pipe(pipeCGI) == -1)
	{
		setStatusCode(INTERNAL_SERVER_ERROR);
		throw CgiError();
	}
	pidCgi = fork();
	if (pidCgi == -1)
	{
		setStatusCode(INTERNAL_SERVER_ERROR);
		close(pipeCGI[0]);
		close(pipeCGI[1]);
		throw CgiError();
	}
	else if (pidCgi == 0)
	{
		if (env["REQUEST_METHOD="] == "POST")
		{
			std::ofstream tempfile("./website/directory/uploads/temp.file");
			FILE * tmpFileWrite = std::fopen("./website/directory/uploads/temp.file", "w");
			if (!tmpFileWrite)
			{
				setStatusCode(INTERNAL_SERVER_ERROR);
				throw CgiError();
			}
			if (this->whichMethod == POST)
			{
				char const * writingRecipient = this->data.c_str();
				if (write(fileno(tmpFileWrite), writingRecipient, this->data.length()) == -1)
				{
					setStatusCode(INTERNAL_SERVER_ERROR);
					throw CgiError();
				}
			}
			close(fileno(tmpFileWrite));

			FILE * tmpFileRead = std::fopen("./website/directory/uploads/temp.file", "r");
			if (!tmpFileRead)
			{
				setStatusCode(INTERNAL_SERVER_ERROR);
				throw CgiError();
			}
			dup2(fileno(tmpFileRead), STDIN_FILENO);
			close(fileno(tmpFileRead));
		}
			
		if (dup2(pipeCGI[1], STDOUT_FILENO) == -1)
		{
			setStatusCode(INTERNAL_SERVER_ERROR);
			throw CgiError();
		}
		close(pipeCGI[0]);
		close(pipeCGI[1]);

		char **args = convertArgs(this->args);
		char **env = convertEnv(this->env);
		// size_t lastSlashChar = this->path.find_last_of("/");
		// int ret = chdir(this->path.substr(0, lastSlashChar).c_str());
		// if (ret == -1)
		// {
		// 	freeTab(args);
		// 	freeTab(env);
		// 	std::exit(1);
		// }
		// std::cout << "ARGS" << std::endl;
		// for (int i = 0; args[i] != NULL; i++)
		// {
		// 	std::cout << args[i] << std::endl;
		// }
		// std::cout << "ENV" << std::endl;
		// for (int i = 0; env[i] != NULL; i++)
		// {
		// 	std::cout << env[i] << std::endl;
		// }
		// std::cout.flush();

		
		int ret = execve(this->path.c_str(), args, env);
		if (ret == -1)
		{
			setStatusCode(INTERNAL_SERVER_ERROR);
			freeTab(args);
			freeTab(env);
			throw CgiError();
		}
	}
	else
	{
		close(pipeCGI[1]);
		
		readPipe(pipeCGI[0]);
		close(pipeCGI[0]);
		
		int status;
		int res = waitpid(pidCgi, &status, 0);
		if (res == -1)
		{
			setStatusCode(INTERNAL_SERVER_ERROR);
			throw CgiError();
		}
		if (WIFEXITED(status))
		{
			int exitcode = WEXITSTATUS(status);
			if (exitcode != 0)
			{
				setStatusCode(INTERNAL_SERVER_ERROR);
				close(pipeCGI[0]);
				throw CgiError();
			}
		}
		if (std::remove("./website/directory/uploads/temp.file") != 0)
		{
			if (errno != ENOENT)
			{
				setStatusCode(INTERNAL_SERVER_ERROR);
				throw CgiError();
			}
		}
		setStatusCode(OK);
	}
}

const char *Cgi::CgiError::what() const throw()
{
    return("Cgi::CgiError : Internal  Error Cgi.");
}

bool	Cgi::isValidScript()
{
	struct stat     statbuf;

	if (stat(path.c_str(), &statbuf) == -1)
	{
		setStatusCode(NOT_FOUND); //! Double check Status Code
		return (false);
	}
	if (!(statbuf.st_mode & S_IXUSR))
	{
		setStatusCode(FORBIDDEN); //! Double check Status Code
		return (false);
	}
	return (true);
}

void	Cgi::setStatusCode(StatusCode code)
{
		status = code;
}

StatusCode	Cgi::getStatusCode() const
{
		return (status);
}

void Cgi::printData()
{
	std::cout << "Data\n" << data << std::endl;
}