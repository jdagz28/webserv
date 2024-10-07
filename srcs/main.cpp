/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 00:23:30 by jdagoy            #+#    #+#             */
/*   Updated: 2024/10/07 15:39:33 by jdagoy           ###   ########.fr       */
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
#include <csignal>

#define PORT    4242    

int server_socket = -1;


std::string getConfigPath(int argc, char **argv)
{
    if (argc == 1)
        return (DEFAULT_CONFIG);
    else
        return (argv[1]);
    return ("");
}

void    signal_handler(int signum)
{
    std::cout << "Received signal " << signum << std::endl;
    std::cout << "===== Shutting down server =====" << std::endl;
    if (server_socket != -1)
    {
        close(server_socket);
        std::cout << "Server socket closed." << std::endl;
    }
    exit(signum);
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
        std::string configPath = getConfigPath(argc, argv);
        
        Config  config(configPath);
        
        struct sockaddr_in server_addr, client_addr;
        socklen_t client_len = sizeof(client_addr);

        signal(SIGINT, signal_handler);
        signal(SIGTERM, signal_handler);

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
        
        printConfigData(config);
        std::cout << "Listening on port: " << PORT << std::endl;
        while (true)
        {
            int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
            if (client_socket < 0)
            {
                perror("Failed to accept connection");
                continue;
            }
            // std::cout << "Connection accepted" << std::endl;

            // std::cout << "Receiving request..." << std::endl;
            HttpRequest request(client_socket);
            // printHttpRequest(request);
            // std::cout << "Request parsed." << std::endl;

            // std::cout << "Generating response..." << std::endl;
            HttpResponse response(request, config, client_socket);
            response.execMethod();
            response.generateHttpResponse();
            
            // printHttpResponse(response.getHttpResponse());
            // std::cout << "Sending response..." << std::endl;
            response.sendResponse();

            close(client_socket);
            // std::cout << "Connection closed" << std::endl;
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

