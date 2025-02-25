/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 23:25:12 by jdagoy            #+#    #+#             */
/*   Updated: 2025/02/25 11:09:07 by jdagoy           ###   ########.fr       */
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
		std::string 							_scriptName;
		std::string								_requestMethod;
		std::map<std::string, std::string> 		_env;
		std::string 							_output;
		std::string 							_path;
		StatusCode								_status;
		std::map<std::string, std::string>      _headers;
		std::map<std::string, std::string>	  	_formData;
		std::string								_body;

		Cgi(const Cgi &copy);
		Cgi &operator=(const Cgi &copy);

		bool	isValidScript();
		void	executeScript();
		char	**generateEnv();
		void	generateFullScriptPath();
		void	parseCGIOutput();
		void	cleanUpScriptName();

	public:
		Cgi();
		Cgi(const std::string &scriptName, const std::string &requestMethod);
		~Cgi();

		void 	setEnv(const std::string &key, const std::string &value);
		void	setRequestMethod(const std::string &requestMethod);
		void 	setStatusCode(StatusCode status);

		void 	prepareEnv();
		void	executeCGI();

		std::map<std::string, std::string> &getHeaders();
		std::string &getBody();
		StatusCode	getStatusCode();
};

#endif