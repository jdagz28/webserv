/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/06 22:38:51 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/25 11:07:07 by jdagoy           ###   ########.fr       */
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
        std::string                                         _path;
        std::map<std::string, std::vector<std::string> >    _directives;
        std::vector<std::string>                            _allowedMethods;
        bool                                                _isLimited;
        std::vector<std::string>                            _limitExcept;
        


    public:
        LocationConfig();
        LocationConfig(const LocationConfig &copy);
        LocationConfig  &operator=(const LocationConfig &copy);
        ~LocationConfig();
        
        void    setDirective(const std::string &directive, const std::string &value);
        void    setPath(const std::string &path);
        void    setAllowedMethod(const std::string &method);
        void    setLimitExcept(bool limited);
        void    setLimitExcept(const std::string &method);

        const std::map<std::string, std::vector<std::string> > &getDirectives() const;
        const std::string &getPath() const;
        const std::string getRoot() const;
        const std::vector<std::string> &getAllowedMethods() const;
        const std::vector<std::string> &getLimitExcept() const;
        const std::string getDefaultName() const;
        const std::string getIndex() const;
        const std::string getAutoIndex() const;
        const std::vector<std::string> &getRedirect() const;

        bool    isMethodAllowed(const std::string &method) const;
        bool    isLimited() const;
        bool    isLimitExcept(const std::string &method) const;
        bool    isRedirect() const;
        
};

#endif