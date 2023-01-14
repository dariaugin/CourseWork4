//
//  server.cpp
//  Course_Work_Project
//
//  Created by Дарья Угнивенко on 12.01.2023.
//  Copyright © 2023 dddd. All rights reserved.
//
/*
#include "server.hpp"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
std::mutex mut;
#define PORT 8080


struct Server{
    int master_socket , addrlen , new_socket , client_socket[30] ,
    max_clients = 50 , activity, valread , sd, maxsd;
    struct sockaddr_in address;
    fd_set readfds;
    std::vector<std::thread> threads;
    
    void start(){
        bind_and_llisten();
        handleConnection();
    }
    
    void bind_and_llisten(){
        if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
            {
                perror("socket failed");
                exit(EXIT_FAILURE);
            }
        
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons( PORT );
            
            
            if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
            {
                perror("bind failed");
                exit(EXIT_FAILURE);
            }
            printf("PORT %d \n", PORT);
            
            if (listen(master_socket, 3) < 0)
            {
                perror("listen failed");
                exit(EXIT_FAILURE);
            }

            addrlen = sizeof(address);
            printf("SERVER waiting for connections ...");
    }
    void handleConnection(){
        for (int i = 0; i < max_clients; i++)
        {
            client_socket[i] = 0;
        }
        while(true)
        {
            FD_ZERO(&readfds);
            FD_SET(master_socket, &readfds);
            maxsd = master_socket;
            
            for ( int i = 0 ; i < max_clients ; i++)
            {
                sd = client_socket[i];

                if(sd > 0)
                    FD_SET( sd , &readfds);
                if(sd > maxsd)
                    maxsd = sd;
            }
            
            activity = select( maxsd + 1 , &readfds , NULL , NULL , NULL);
            
            if ((activity < 0) && (errno!=EINTR)){
                    printf("select failed");
            }
                
            
            if (FD_ISSET(master_socket, &readfds))
                {
                    handleNewConnection();
                }
                
            }
        
        
    }
    void startNewThreadTeacher(int ns){

        Test t;
        auto th = std::thread(&Test::createTest,t,ns);
        th.detach();
        
    }
    void startNewThreadStudent(int ns, int id){
        
        if(id < myTests.size() && id >= 0){
        int status[1] = {1};
        write(ns, status, sizeof(status));
        Test t = myTests[id];
        Student_Test_Session student(ns, myTests[id]);
        auto th = std::thread(&Student_Test_Session::start_Test_Session,student);
        th.detach();
        client_socket[ns] = 0;
        }else{
            int status[1] = {0};
            write(ns, status, sizeof(status));
            close( sd );
            client_socket[ns] = 0;
        }
        
    }
    
    
    void handleNewConnection(){
        if ((new_socket = accept(master_socket,
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            
            if (new_socket > maxsd) {
                maxsd = new_socket;
                
            }
        
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs
                (address.sin_port));
                
            int status[2];
        
                if(read( new_socket, status, sizeof(status))){
                   
                    if(status[0] == 0){
                        
                        startNewThreadTeacher(new_socket);
                    }else{
                        startNewThreadStudent(new_socket, status[1]);
                    }
                }else{
                    perror("unknown connection");
                    exit(EXIT_FAILURE);
                }

        }
}*/

