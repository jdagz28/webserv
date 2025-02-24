/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 23:18:08 by jdagoy            #+#    #+#             */
/*   Updated: 2025/02/24 12:01:22 by jdagoy           ###   ########.fr       */
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
}

void	Cgi::executeCGI()
{
	try
	{
		if (!isValidScript())
		{
			std::cout << "Script Invalid" << std::endl; //! DELETE
			setStatusCode(BAD_REQUEST); //! Double check Status Code
			return ;
		}
		std::cout << "Executing CGI script" << std::endl;
		executeScript(); //! Create Child Process 
		// parseCGIOutput(); //! Read from pipe
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
		return (false);
	if (!(statbuf.st_mode & S_IXUSR)) 
		return (false);
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
		dup2(pipe_in[0], STDIN_FILENO);
		dup2(pipe_out[1], STDOUT_FILENO);
		
		close(pipe_in[1]);
		close(pipe_out[0]);

		char **env = generateEnv();
		if (env == NULL)
		{
			setStatusCode(INTERNAL_SERVER_ERROR);
			throw std::runtime_error("Error: Failed to generate environment variables");
		}
		// std::cout << "Script Path: " << _path << std::endl;
		//! arguments from request body; parsing
		char *argv[] = {const_cast<char *>(_path.c_str()), NULL};
		
		execve(argv[0], argv, env);
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
		close (pipe_out[0]);

		// wait for child to finish
		int status;
		waitpid(pid, &status, 0);
	
		_output = ss.str();
		std::cout << _output << std::endl;
	}
}