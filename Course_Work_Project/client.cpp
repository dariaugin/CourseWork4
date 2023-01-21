#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <vector>

#define PORT 8080

void getResults(int sockfd){
    char mes[90000];
    read(sockfd, mes, sizeof(mes));
    std::cout<<mes<<"\n";
    
}

void startUserSession(int sockfd){
    
    int status[1] = {1};
    write(sockfd, status, sizeof(status));
    char mes[5000];
    read(sockfd, mes, sizeof(mes));
    std::cout<<mes<<"\n";
    std::string word;
    std::getline (std::cin,word);
    char const *q = word.c_str();
    write(sockfd, q, sizeof(q));
    
    getResults(sockfd);

}


int main(int argc, const char * argv[]) {

    int sockfd;
    struct sockaddr_in     servaddr;
    
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        perror("Can't create a socket. Some error has occured");
        exit(EXIT_FAILURE);
    }
 
     servaddr.sin_family = AF_INET;
     servaddr.sin_addr.s_addr = INADDR_ANY;
     servaddr.sin_port = htons(PORT);
    
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(EXIT_FAILURE);
    }
    
    startUserSession(sockfd);
    close(sockfd);
    return 0;
}
