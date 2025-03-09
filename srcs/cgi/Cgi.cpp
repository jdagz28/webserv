/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 19:52:54 by romvan-d          #+#    #+#             */
/*   Updated: 2025/03/09 14:41:34 by jdagoy           ###   ########.fr       */
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

Cgi::Cgi(const HttpRequestLine & requestLine, const HttpRequest & request, const std::string path, const std::string &uploadDir, 
			const std::string &body, const std::string &programPath, int timeout)
	: status(OK), cgiOutput(""), outputBody(""), programPath(programPath), timeout(timeout)
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
		throw CgiError();
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
			throw CgiError();
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
	char ** argstable = new char*[size + 2];
	if (!argstable)
	{
		setStatusCode(INTERNAL_SERVER_ERROR);
		throw CgiError();
	}
	size_t j = 0;
	for (std::vector<std::string>::iterator i = args.begin(); i != args.end(); ++i)
	{
		std::cout << *i << std::endl;
		argstable[j] = new char[i->length() + 1];
		if (!argstable[j])
		{
			setStatusCode(INTERNAL_SERVER_ERROR);
			freeTab(argstable);
			throw CgiError();
		}
		strcpy(argstable[j], i->c_str()); //! FORBIDDEN FUNCTION
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
}

void Cgi::runCgi()
{
	if (!isValidInterpreterAndScript())
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
			tempFilePath();
			std::ofstream tempfile(tempFile.c_str());
			FILE * tmpFileWrite = std::fopen(tempFile.c_str(), "w");
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

			FILE * tmpFileRead = std::fopen(tempFile.c_str(), "r");
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
		
		int ret = execve(args[0], args, env);
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
				
		time_t start = time(NULL);		
		int status;
		int res;
		
		while (true)
		{
			res = waitpid(pidCgi, &status, WNOHANG);
			if (res == -1)
			{
				if (errno == EINTR)
            		continue;
				setStatusCode(INTERNAL_SERVER_ERROR);
				throw CgiError();
			}
			else if (res == pidCgi)
				break ; 
			if (time(NULL) - start >= timeout)
			{
				kill(pidCgi, SIGKILL);
				setStatusCode(GATEWAY_TIMEOUT);
				return ;
			}
		}

		readPipe(pipeCGI[0]);
		close(pipeCGI[0]);
		
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
		if (std::remove(tempFile.c_str()) != 0)
		{
			if (errno != ENOENT)
			{
				setStatusCode(INTERNAL_SERVER_ERROR);
				throw CgiError();
			}
		}
		setStatusCode(OK);
		parseCgiOutput();
	}
}

const char *Cgi::CgiError::what() const throw()
{
    return("Cgi::CgiError : Internal  Error Cgi.");
}

bool	Cgi::isValidInterpreterAndScript()
{
	parseShebangInterpreter();
	
	struct stat     statbuf;

	if (stat(args[0].c_str(), &statbuf) == -1)
	{
		setStatusCode(BAD_REQUEST);
		return (false);
	}
	
	if (stat(path.c_str(), &statbuf) == -1)
	{
		setStatusCode(BAD_REQUEST); 
		return (false);
	}
	
	if (!(statbuf.st_mode & S_IXUSR))
	{
		setStatusCode(FORBIDDEN); 
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

void	Cgi::parseCgiOutput() 
{
	if (cgiOutput.find("text/html") != std::string::npos)
	{
		size_t htmlPos = cgiOutput.find("text/html");
		if (htmlPos == std::string::npos)
				outputBody = cgiOutput;
		else
		{
			std::string headers = cgiOutput.substr(0, htmlPos + std::string("text/html").length());
			outputBody = cgiOutput.substr(htmlPos + std::string("text/html").length());
			std::stringstream ss(headers);
			std::string key;
			std::string value;
			while (std::getline(ss, key, ':'))
			{
				std::getline(ss, value, '\n');
				outputHeaders[key] = value;
			}
		}
	}
	else
	{
		size_t pos = cgiOutput.find("\n\n");
		if (pos == std::string::npos)
			outputBody = cgiOutput;
		else
		{
			std::string headers = cgiOutput.substr(0, pos + 2);
			outputBody = cgiOutput.substr(pos + 2);
			std::stringstream ss(headers);
			std::string key;
			std::string value;
			while (std::getline(ss, key, ':'))
			{
				std::getline(ss, value, '\n');
				trimWhitespaces(key);
				trimWhitespaces(value);
				outputHeaders[key] = value;
			}
		}
	}
}

std::map<std::string, std::string> Cgi::getOutputHeaders() const
{
	return (outputHeaders);
}

std::string Cgi::getOutputBody() const
{
	return (outputBody);
}

void	Cgi::tempFilePath()
{
	if (uploadDir.empty())
		uploadDir = UPLOAD_DIR;
	
	struct stat statbuf;

	if (stat(uploadDir.c_str(), &statbuf) == -1)
	{
		setStatusCode(BAD_REQUEST);
		throw CgiError();
	}
	tempFile = uploadDir;
	if (tempFile[tempFile.length() - 1] != '/')
    	tempFile.push_back('/');
	tempFile += "temp.file";
}

void	Cgi::parseShebangInterpreter()
{
	std::ifstream script(path.c_str());
	if (!script)
	{
		setStatusCode(INTERNAL_SERVER_ERROR); 
		throw CgiError();
	}	
	
	if (!programPath.empty())
		args.push_back(programPath);
	else
	{
		std::string shebangLine;
		if (!std::getline(script, shebangLine))
		{
			script.close();
			setStatusCode(INTERNAL_SERVER_ERROR);
			throw CgiError();
		}
		script.close();

		if (shebangLine[0] != '#' || shebangLine[1] != '!') 
		{
			setStatusCode(INTERNAL_SERVER_ERROR);
			throw CgiError();
		}

		shebangLine = shebangLine.substr(2);

		std::istringstream iss(shebangLine);
		std::string token;
		while (iss >> token)
			args.push_back(token);
	}
	args.push_back(path);
}