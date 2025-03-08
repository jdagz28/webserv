/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 17:08:50 by romvan-d          #+#    #+#             */
/*   Updated: 2025/03/09 00:10:46 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

#include "HttpRequest.hpp"
#include "HttpRequestLine.hpp"
#include "LocationConfig.hpp"
#include <exception>
#include <map>
#include <unistd.h>
#include <vector>

class Cgi
{
	public :
	
		Cgi();
		Cgi(const HttpRequestLine &requestLine, const HttpRequest & request, const std::string path, 
				const std::string &uploadDir, const std::string &body, const std::string &programPath);
        Cgi(const Cgi &other);
		~Cgi();

		Cgi &operator=(const Cgi &other);
		void	runCgi();
		void	parseCgiOutput();
		
		StatusCode getStatusCode() const;
		std::map<std::string, std::string> getOutputHeaders() const;
		std::string getOutputBody() const;

		void	printData();

		class CgiError : public std::exception
		{
			const char * what() const throw();
		};

	private :

		size_t								whichMethod;
		std::string							data;
		std::vector<std::string> 			args;
		std::map <std::string, std::string> env;
		std::string							path;
		std::string							uploadDir;
		StatusCode							status;
		std::string							cgiOutput;
		std::map<std::string, std::string>	outputHeaders;
		std::string							outputBody;
		std::string							tempFile;
		std::string							programPath;
		
		char ** convertEnv(std::map<std::string, std::string> env);
		char ** convertArgs(std::vector<std::string> args);
		void	readPipe(int pipeRead);
		bool	isValidInterpreterAndScript();
		void 	setStatusCode(StatusCode coe);
		void	tempFilePath();
		void	parseShebangInterpreter();
};

#endif