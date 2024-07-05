/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 22:38:46 by jdagoy            #+#    #+#             */
/*   Updated: 2024/07/05 03:35:05 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

#define DEFAULT_CONFIG "./conf/default.conf"


class   Config
{
    private:
        bool            error;
        std::string     errorMsg;

        bool    validExtension(const std::string &configPath);
        void    checkFile(const std::string &configPath);
    
    public:
        Config(const std::string &configPath);
        Config(const Config &copy);
        ~Config();
        Config  &operator=(const Config &copy);

        bool     isError() const;
};

#endif 