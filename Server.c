#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>

//SET UP SOCKET FD AND RETURN IT.
int setup_socket(){

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd == -1){
        perror("socket");
        exit(1);
    }

    int yes = 1;
    int sso = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    if(sso == -1){
        perror("setsockopt");
        exit(1);
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    int bindfd = bind(sockfd,(struct sockaddr *)&addr, sizeof(addr));
    if(bindfd == -1){
        perror("bind");
        exit(1);
    }

    return sockfd;
}

int getconnection(int sockfd){
     //GETTING CONNECTION
    printf("WAITING\n");
    int wait = listen(sockfd, 1);
    if(sockfd == -1){
        perror("listen");
        exit(1);
    }

    
    int connectfd = accept(sockfd, NULL, NULL);
    if(connectfd == -1){
        perror("connection");
        exit(1);    
    }

    printf("Connection\n");

    return connectfd;
}

//error handling
int create_header(char* msg, size_t msg_size, long size){
    int len = snprintf(msg, msg_size,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %ld\r\n"
        "\r\n", size);

    return len;
}

ssize_t write_http(int connection_fd, char* header, size_t header_size, char* body, size_t body_size){
    //WRITING HEADER THEN BODY
    ssize_t total = 0;

    ssize_t wrote = write(connection_fd, header, header_size);
    if(wrote == -1){
        perror("write");
        exit(1);
    }
    printf("wrote header %zd bytes\n", wrote);

    total += wrote;

    wrote = write(connection_fd, body, body_size);
     if(wrote == -1){
        perror("write");
        exit(1);
    }

    printf("wrote body %zd bytes\n", wrote);

    total += wrote;

    return total;


}