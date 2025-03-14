/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 22:38:51 by jdagoy            #+#    #+#             */
/*   Updated: 2025/03/09 01:12:04 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include <string>
#include <vector>
#include <map>


class LocationConfig
{
    private:
        std::string                             _path;
        std::map<std::string, std::string>      _directives;
        std::vector<std::string>                _allowedMethods;
        std::vector<std::string>                _cgiExtensions;
		std::vector<std::string>				_denyMethods;
		std::string 							_maxBodyMode;
     
    public:
        LocationConfig();
        LocationConfig(const LocationConfig &copy);
        LocationConfig  &operator=(const LocationConfig &copy);
        ~LocationConfig();
        
        void    setDirective(const std::string &directive, const std::string &value);
        void    setPath(const std::string &path);
        void    setAllowedMethod(const std::string &method);
        void    setCGIExtension(const std::string &extension);
		void    setDenyMethod(const std::string &method);
		void    setMaxBodyMode(const std::string &mode);

        const std::map<std::string, std::string >	&getDirectives() const;
        const std::string	getPath() const;
        const std::string	getRoot() const;
        const std::vector<std::string>	&getAllowedMethods() const;
        const std::string	getDefaultName() const;
        const std::string	getIndex() const;
        const std::string	getAutoIndex() const;
        const std::vector<std::string>	&getCGIExtensions() const;
        std::string getRedirect()	const;
        size_t getClientMaxBodySize();
		const std::vector<std::string>	&getDenyMethods() const;
		const std::string	getMaxBodyMode() const;
        const std::string   getUploadDir() const;
        const std::string   getProgram() const;
		std::string	getCgiTimeout() const;

        bool    isMethodAllowed(const std::string &method) const;
        bool    isRedirect() const;
        bool    isPathAlreadySet(const std::string &path) const;
        bool    isLimitExcept() const;
		bool	isDenyMethod(const std::string &method) const;
		bool	isCGIDirectiveSet() const;
		bool	isCGIMode() const;
        bool	isCGIExtensionAllowed(const std::string &extension) const;
};

#endif