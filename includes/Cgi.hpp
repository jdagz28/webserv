/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 23:25:12 by jdagoy            #+#    #+#             */
/*   Updated: 2025/02/22 21:53:24 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "webserv.hpp"
#include <string>
#include <map>


class Cgi
{
	private:
		std::string 						_path;
		std::map<std::string, std::string> 	_env;
		std::string 						_output;

		Cgi(const Cgi &copy);
		Cgi &operator=(const Cgi &copy);

		bool	isValidScript();

	public:
		Cgi();
		Cgi(const std::string &path, const std::map<std::string, std::string> &env);
		~Cgi();

		void 	setScriptPath(const std::string &path);
		void 	setEnv(const std::map<std::string, std::string> &env);

		void	executeCGI();
};

#endif