/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romvan-d <romvan-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 17:08:50 by romvan-d          #+#    #+#             */
/*   Updated: 2025/02/25 15:17:02 by romvan-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

#include "Httprequest.hpp"
#include "LocationConfig.hpp"
#include <exception>
#include <map>
#include <unistd.h>
#include <vector>

class Cgi
{
	public :
	
		Cgi();
		Cgi(HttpRequestLine &request, std::string path, std::string uploadDir);
        Cgi(const Cgi &other);
		~Cgi();

		Cgi &operator=(const Cgi &other);
		std::string run();

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


		char ** convertEnv(std::map<std::string, std::string>);
		char ** convertArgs(std::vector<std::string>);
		std::string readPipe(int pipeRead);

};