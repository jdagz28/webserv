/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_http.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 00:23:30 by jdagoy            #+#    #+#             */
/*   Updated: 2024/08/01 01:57:51 by jdagoy           ###   ########.fr       */
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

#define PORT    4242    

int main(int argc, char **argv)
{   
    (void) argc;
    (void) argv;
    int server_socket, client_socket;
    struct  sockaddr_in server_addr, client_addr;
    socklen_t   client_len = sizeof(client_addr);
    
    //create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        std::cerr << "Error: failed to create socket" << std::endl;
        return (1);
    }

    // configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // bind socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cerr << "Error: failed to bind socket" << std::endl;
        close(server_socket);
        return (1);
    }
    
    // listen
    if (listen(server_socket, 5) == -1)
    {
        std::cerr << "Error: Could not listen to socket." << std::endl;
        close(server_socket);
        return (1);
    }   
    
    std::cout << "Listining on port: " << PORT << std::endl;


    //accept connections
    while (true)
    {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0)
        {
            std::cerr << "Failed to accept connection" << std::endl;
            continue ; 
        }
        
        std::cout << "Connection accepted" << std::endl;
        
        HttpRequest request;
        request.requestToBuffer(client_socket);
        request.printBuffer();
        
        
        close(client_socket);
    }

    close(server_socket);
    return (EXIT_SUCCESS);
}

