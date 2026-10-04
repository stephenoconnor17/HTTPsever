#include <stdio.h>
#include <stdlib.h>

#include "Server.h"
#include "FileHandler.h"

#include <unistd.h>

#define MAX_HEADER_SIZE 256

int main(){

    int sockfd = setup_socket();

    long body_size;
    char* body = readin_file("index.html", &body_size);

    int connectfd = getconnection(sockfd);

    //"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 26\r\n\r\n";
    char header[MAX_HEADER_SIZE]; 
    int header_len = create_header(header, MAX_HEADER_SIZE, body_size);

    ssize_t bytes_written = write_http(connectfd, header, header_len, body, body_size);

    printf("total bytes written: %zd\n", bytes_written);

    close(connectfd);
    close(sockfd);
    return 0;
}