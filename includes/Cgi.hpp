/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 17:08:50 by romvan-d          #+#    #+#             */
/*   Updated: 2025/03/07 16:09:23 by jdagoy           ###   ########.fr       */
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
		Cgi(const HttpRequestLine &requestLine, const HttpRequest & request, const std::string path, const std::string &uploadDir);
        Cgi(const Cgi &other);
		~Cgi();

		Cgi &operator=(const Cgi &other);
		std::string runCgi();
		
		StatusCode getStatusCode() const;
		void	printMultiFormData();

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
		std::map<std::string, MultiFormData>  			uploadData;

		
		char ** convertEnv(std::map<std::string, std::string> env);
		char ** convertArgs(std::vector<std::string> args);
		std::string readPipe(int pipeRead);
		bool	isValidScript();
		void 	setStatusCode(StatusCode coe);
};

#endif