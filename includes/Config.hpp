/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 22:38:46 by jdagoy            #+#    #+#             */
/*   Updated: 2024/07/05 22:27:32 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <exception>
#include <utility>

#define DEFAULT_CONFIG "./conf/default.conf"


class Config
{
    private:
        std::pair<int, std::string>     error;
        
        bool    validPath(const std::string &configPath);
        bool    validExtension(const std::string &configPath);
        void    checkFile(const std::string &configPath);
    
    public:
        Config(const std::string &configPath);
        Config(const Config &copy);
        ~Config();
        Config  &operator=(const Config &copy);

        bool     isError() const;

        class configException : public std::exception
        {
            private:
                std::string exceptMsg;
            public:
                configException(const std::string &msg) : exceptMsg(msg) {};
                ~configException() throw() {};
                const char *what() const throw();
        };

};

#endif 