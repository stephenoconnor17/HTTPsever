#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(){

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

    /*FILE READING*/
    FILE* fp = fopen("index.html", "rb");
    if(!fp){
        perror("fopen");
        exit(1);
    }

    fseek(fp,0,SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    char* body = malloc(size);
    fread(body, 1, size, fp);
    fclose(fp);

    //"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 26\r\n\r\n";
    char msg[256]; 
    int len = snprintf(msg, sizeof(msg),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %ld\r\n"
            "\r\n", size);

    ssize_t wrote = write(connectfd,msg,len);
    if(wrote == -1){
        perror("write");
        exit(1);
    }
    printf("wrote header %zd bytes\n", wrote);

    wrote = write(connectfd, body,size);
     if(wrote == -1){
        perror("write");
        exit(1);
    }
    printf("wrote body %zd bytes\n", wrote);


    close(connectfd);
    close(sockfd);
    return 0;
}