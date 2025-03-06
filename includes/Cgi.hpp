/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romvan-d <romvan-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 17:08:50 by romvan-d          #+#    #+#             */
/*   Updated: 2025/03/03 18:17:26 by romvan-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

#include "HttpRequest.hpp"
#include "LocationConfig.hpp"
#include <exception>
#include <map>
#include <unistd.h>
#include <vector>

class Cgi
{
	public :
	
		Cgi();
		Cgi(HttpRequestLine &requestLine, HttpRequest & request, std::string path, std::string uploadDir);
        Cgi(const Cgi &other);
		~Cgi();

		Cgi &operator=(const Cgi &other);
		std::string runCgi();

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


		char ** convertEnv(std::map<std::string, std::string> env);
		char ** convertArgs(std::vector<std::string> args);
		std::string readPipe(int pipeRead);

};

#endif