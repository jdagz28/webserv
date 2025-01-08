/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagz28 <jdagz28@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 00:23:30 by jdagoy            #+#    #+#             */
/*   Updated: 2025/01/08 13:24:21 by jdagz28          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include "Config.hpp"
#include "debug.hpp"
#include "webserv.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Logger.hpp"

// #define PORT   4242 //1919    

int server_socket = -1;


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

    Logger log;
    try
    {
        std::string configPath = getConfigPath(argc, argv);
        
        Config  config(configPath);

        
        /*
         ! Moved to Socket
        struct sockaddr_in server_addr, client_addr;
        socklen_t client_len = sizeof(client_addr);

        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket < 0)
        {
            std::cerr << "Error: failed to create socket" << std::endl;
            return (1);
        }

        int opt = 1;
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        {
            std::cerr << "Error: setsockopt failed" << std::endl;
            close(server_socket);
            return 1;
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(PORT);

        if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        {
            std::cerr << "Error: failed to bind socket" << std::endl;
            close(server_socket);
            return (1);
        }
        
        if (listen(server_socket, 5) == -1)
        {
            std::cerr << "Error: Could not listen to socket." << std::endl;
            close(server_socket);
            return (1);
        }
        */   
        
        // printConfigData(config);
        log.checkConfig(config);
        while (true)
        {
            /* //! Moved to Socket
            int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
            if (client_socket < 0)
            {
                perror("Failed to accept connection");
                continue;
            }
            */

            HttpRequest request(client_socket);
            log.request(request);
            // printHttpRequest(request);
            
            HttpResponse response(request, config, client_socket);

            log.response(response);
            response.sendResponse();

            // close(client_socket); //! Moved to Socket
            // std::cout << "Connection closed" << std::endl;
        }

        close(server_socket);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        log.configError(e.what());
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}

