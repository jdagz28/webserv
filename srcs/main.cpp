/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 00:23:30 by jdagoy            #+#    #+#             */
/*   Updated: 2024/07/05 03:24:41 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstdlib>
#include <string>
#include "Config.hpp"


std::string getConfigPath(int argc, char **argv)
{
    if (argc == 1)
        return (DEFAULT_CONFIG);
    else
        return (argv[1]);
    return ("");
}

int main(int argc, char **argv)
{
    if (argc != 1 && argc != 2)
    {
        std::cerr << "Usage: ./webserv OR ./webserv {configFile.conf}" << std::endl;
        return (EXIT_FAILURE);
    }

    std::string configPath = getConfigPath(argc, argv);
    
    
    Config  config(configPath);
    return (EXIT_SUCCESS);
}

