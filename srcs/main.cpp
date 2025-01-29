/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 00:23:30 by jdagoy            #+#    #+#             */
/*   Updated: 2025/01/10 14:58:13 by jdagoy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <cstring>
#include "Config.hpp"
#include "debug.hpp"
#include "webserv.hpp"
#include "Server.hpp"

// #define PORT   4242 //1919    

int server_socket = -1;

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}	
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

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

    try
    {
		fd_set	master_fd;
		fd_set	read_fds;
		int		fd_max;
		timeval	timeout;
		int		new_fd;
		
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;
        std::string configPath = getConfigPath(argc, argv);
        
        Config  config(configPath);
        // printConfigData(config);

        Server server(config);
        
        server.initServer();
        server.runServer();
    }
	close(server_socket);
	}
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}
