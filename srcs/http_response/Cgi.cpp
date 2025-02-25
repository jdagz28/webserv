/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 23:18:08 by jdagoy            #+#    #+#             */
/*   Updated: 2025/02/25 11:46:52 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
#include "webserv.hpp"
#include <iostream>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>
#include <sstream>
#include <sys/wait.h>
#include <cstdlib>

Cgi::Cgi()
{}

Cgi::Cgi(const std::string &scriptName, const std::string &requestMethod)
	: _scriptName(scriptName), _requestMethod(requestMethod), _status(OK)
{
	try
	{
		if (_scriptName.find("?") != std::string::npos)
			cleanUpScriptName();
		generateFullScriptPath();
		std::cout << _path << std::endl; //! DELETE
		prepareEnv();
		executeCGI();
	}
	catch(const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}

Cgi::~Cgi()
{}

void	Cgi::cleanUpScriptName()
{
	if (_scriptName.find("?") != std::string::npos)
	{
		std::string args = _scriptName.substr(_scriptName.find("?") + 1);
		std::stringstream iss(args);
		std::string key;
		std::string value;
		while (std::getline(iss, key, '='))
		{
			std::getline(iss, value, '&');
			_formData[key] = value;
		}
		_scriptName = _scriptName.substr(0, _scriptName.find("?"));
		_env["QUERY_STRING"] = args;
	}
	// for (std::map<std::string, std::string>::iterator it = _formData.begin(); it != _formData.end(); it++)
	// {
	// 	std::cout << it->first << ": " << it->second << std::endl;
	// }	//!DELETE
}

void	Cgi::generateFullScriptPath()
{
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		std::string basePath(cwd);
		std::string fullPath = basePath + "/website/resources/cgi-bin" + _scriptName;
		_path = fullPath;
	}
	else
	{
		setStatusCode(INTERNAL_SERVER_ERROR);
		throw std::runtime_error("Error: Failed to get current working directory");
	}
}

void	Cgi::setRequestMethod(const std::string &requestMethod)
{
	_requestMethod = requestMethod;
}

void 	Cgi::setEnv(const std::string &key, const std::string &value)
{
	_env[key] = value;
}

void 	Cgi::setStatusCode(StatusCode status)
{
	_status = status;
}

void 	Cgi::prepareEnv()
{
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["REQUEST_METHOD"] = _requestMethod;
	_env["SCRIPT_FILENAME"] = _scriptName;
	_env["SCRIPT_PATH"] = _path;
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["UPLOAD_DIR"] = "/website/directory/uploads";
	if (_env.find("CONTENT_LENGTH") == _env.end())
		_env["CONTENT_LENGTH"] = "NULL";
}

void	Cgi::executeCGI()
{
	try
	{
		if (!isValidScript())
		{
			std::cout << "Script Invalid" << std::endl; //! DELETE
			return ;
		}
		std::cout << "Executing CGI script" << std::endl;
		executeScript(); //! Create Child Process 
		parseCGIOutput(); //! Read from pipe
	}
	catch(const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	
}

bool	Cgi::isValidScript()
{
	struct stat 	statbuf;

	if (stat(_path.c_str(), &statbuf) == -1) 
	{
		setStatusCode(NOT_FOUND); //! Double check Status Code
		return (false);
	}
	if (!(statbuf.st_mode & S_IXUSR))
	{
		setStatusCode(BAD_REQUEST); //! Double check Status Code
		return (false);
	}
	return (true);
}

char	**Cgi::generateEnv()
{
	char **env = new char*[_env.size() + 1];
	std::map<std::string, std::string>::iterator it;
	int i = 0;
	for (it = _env.begin(); it != _env.end(); it++)
	{
		std::string env_var = it->first + "=" + it->second;
		env[i] = strdup(env_var.c_str());
		i++;
	}
	env[i] = NULL;
	return (env);
}

void	Cgi::executeScript()
{
	int pipe_in[2];
	int pipe_out[2];
	if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1)
	{
		setStatusCode(INTERNAL_SERVER_ERROR);
		throw std::runtime_error("Error: Failed to create pipe");
	}
	pid_t pid = fork();
	if (pid == -1)
	{
		setStatusCode(INTERNAL_SERVER_ERROR);
		throw std::runtime_error("Error: Failed to fork process");
	}
	if (pid == 0) //! Child Process
	{
		std::cout << "Child Process" << std::endl;
		if (dup2(pipe_in[0], STDIN_FILENO) == -1 || dup2(pipe_out[1], STDOUT_FILENO) == -1)
		{
			setStatusCode(INTERNAL_SERVER_ERROR);
			throw std::runtime_error("Error: Failed to duplicate file descriptor");
		}
		
		close(pipe_in[1]);
		close(pipe_out[0]);

		char **env = generateEnv();
		if (env == NULL)
		{
			setStatusCode(INTERNAL_SERVER_ERROR);
			throw std::runtime_error("Error: Failed to generate environment variables");
		}
		char **argv = generateArgs();
		
		if (execve(argv[0], argv, env) == -1)
		{
			setStatusCode(INTERNAL_SERVER_ERROR);
			throw std::runtime_error("Error: Failed to execute script");
		}
		delete [] argv;
		delete [] env;
		exit(1);
	}
	else
	{
		std::cout << "Parent Process" << std::endl;
		close(pipe_in[0]);
		close(pipe_out[1]);
		close(pipe_in[1]);

		// Read child output STDOUT
		char buffer[1024];
		std::stringstream ss;
		ssize_t bytesRead;
		while ((bytesRead  = read(pipe_out[0], buffer, 1024)) > 0)
		{
			buffer[bytesRead] = '\0';
			ss << buffer;
		}
		if (bytesRead == -1)
		{
			setStatusCode(INTERNAL_SERVER_ERROR);
			throw std::runtime_error("Error: Failed to read from pipe");
		}
		close (pipe_out[0]);

		// wait for child to finish
		int status;
		if (waitpid(pid, &status, 0) == -1)
		{
			setStatusCode(INTERNAL_SERVER_ERROR);
			throw std::runtime_error("Error: Failed to wait for child process");
		}
	
		_output = ss.str();
		std::cout << _output << std::endl;
	}
}

char	**Cgi::generateArgs()
{
	size_t	argCount = _formData.size() + 1;
	char	**argv = new char*[argCount + 2];

	argv[0] = strdup(_path.c_str());
	
	std::map<std::string, std::string>::iterator it;
	int i = 1;
	for (it = _formData.begin(); it != _formData.end(); it++)
	{
		std::string arg = it->first + "=" + it->second;
		argv[i] = strdup(arg.c_str());
		i++;
	}
	argv[i] = NULL;
	return (argv);
}

void	Cgi::parseCGIOutput()
{
	//! Parse headers
	std::cout << "Parsing CGI Output" << std::endl;
	std::string keywordCGI = "<html>"; //! Check 

	size_t headerEnd = _output.find(keywordCGI);
	if (headerEnd == std::string::npos)
	{
		setStatusCode(BAD_REQUEST); //! Check status
		return ; 
	}
	std::string headers = _output.substr(0, headerEnd);
	
	std::istringstream iss(headers);
	std::string line;
	while (std::getline(iss, line))
	{
		size_t colonPos = line.find(":");
		if (colonPos != std::string::npos)
		{
			std::string key = line.substr(0, colonPos);
			std::string value = line.substr(colonPos + 1);
			trimWhitespaces(key);
			trimWhitespaces(value);
			_headers[key] = value;
		}
	}
	_body = _output.substr(headerEnd);

	std::map<std::string, std::string>::iterator it;
	for (it = _headers.begin(); it != _headers.end(); it++)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
	std::cout << "BODY" << std::endl;
	std::cout << _body << std::endl;
}

std::map<std::string, std::string> &Cgi::getHeaders()
{
	return (_headers);
}

std::string &Cgi::getBody()
{
	return (_body);
}

StatusCode	Cgi::getStatusCode()
{
	return (_status);
}