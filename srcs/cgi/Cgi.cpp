/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 19:52:54 by romvan-d          #+#    #+#             */
/*   Updated: 2025/03/07 16:13:09 by jdagoy           ###   ########.fr       */
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

Cgi::Cgi(const HttpRequestLine & requestLine, const HttpRequest & request, const std::string path, const std::string &uploadDir)
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
		this->data = "";//need the body + '\0';
		this->uploadData = request.getMultiFormData();
		this->env["CONTENT_LENGTH="] = request.getHeader("content-length");
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

	// std::map<std::string, std::string>::const_iterator it;
	// for (it = this->env.begin(); it != this->env.end(); it++)
	// {
	// 	std::cout << it->first << it->second << std::endl;
	// }
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

std::string Cgi::readPipe(int pipeRead)
{
	int readChars;
	std::string output = "";
	char buffer[BUFFERSIZE];
	
	while ((readChars = read(pipeRead, buffer, BUFFERSIZE)))
	{
		if (readChars == -1)
		{
			setStatusCode(INTERNAL_SERVER_ERROR);
			throw CgiError();
		}
		output.append(buffer,readChars);
	}
	std::cout << output << std::endl;
	std::cout.flush();
	return output;
}

std::string Cgi::runCgi()
{
	int pipeCGI[2];
	pid_t pidCgi;
	std::string cgiOutput;
	if (pipe(pipeCGI) == -1)
	{
		setStatusCode(INTERNAL_SERVER_ERROR);
		std::cerr << "pipe error" << std::endl;
		throw CgiError();
	}
	pidCgi = fork();
	if (pidCgi == -1)
	{
		setStatusCode(INTERNAL_SERVER_ERROR);
		std::cerr << "fork error" << std::endl;
		close(pipeCGI[0]);
		close(pipeCGI[1]);
		throw CgiError();
	}
	else if (pidCgi == 0)
	{
		// FILE * tmpFileWrite = std::fopen("/tmp/CgiDataUpload", "w");
		// if (!tmpFileWrite)
		// {
		// 	std::exit(1);
		// }
		// if (this->whichMethod == POST)
		// {
		// 	char const * writingRecipient = this->data.c_str();
		// 	if (write(fileno(tmpFileWrite), writingRecipient, this->data.length()) == -1)
		// 	{
		// 		std::exit(1);
		// 	}
		// }
		// close(fileno(tmpFileWrite));

		// FILE * tmpFileRead = std::fopen("/tmp/CgiDataUpload", "r");
		// dup2(fileno(tmpFileRead), STDIN_FILENO);
		// close(fileno(tmpFileRead));

		if (dup2(pipeCGI[1], STDOUT_FILENO) == -1)
		{
			setStatusCode(INTERNAL_SERVER_ERROR);
			std::cerr << "dup2 error" << std::endl;
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
		std::cout << "ENV" << std::endl;
		for (int i = 0; env[i] != NULL; i++)
		{
			std::cout << env[i] << std::endl;
		}
		std::cout.flush();

		
		int ret = execve(this->path.c_str(), args, env);
		if (ret == -1)
		{
			setStatusCode(INTERNAL_SERVER_ERROR);
			std::cerr << "execve error" << std::endl;
			freeTab(args);
			freeTab(env);
			std::exit(1);
		}
	}
	else
	{
		close(pipeCGI[1]);
		
		cgiOutput = readPipe(pipeCGI[0]);
		close(pipeCGI[0]);
		
		int status;
		int res = waitpid(pidCgi, &status, 0);
		if (res == -1)
		{
			setStatusCode(INTERNAL_SERVER_ERROR);
			std::cerr << "waitpid error" << std::endl;
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
		std::cout << cgiOutput << std::endl;
		setStatusCode(OK);
		return cgiOutput;
	}
	std::remove("/tmp/CgiDataUpload");
	return "";
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

void	Cgi::printMultiFormData()
{
	std::map<std::string, MultiFormData>::iterator it;
	for (it = uploadData.begin(); it != uploadData.end(); it++)
	{
		std::cout << it->first << std::endl;
		for (size_t i = 0; i < it->second.binary.size(); i++)
		{
			std::cout << it->second.binary[i];
		}
	}
	std::cout.flush();
}