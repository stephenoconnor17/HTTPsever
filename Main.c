#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Server.h"
#include "FileHandler.h"
#include "Tokeniser.h"
#include "HTTP.h"

#include <unistd.h>

#define MAX_HEADER_SIZE 256

int main(){

    int sockfd = setup_socket();

    long body_size;
    char* body = readin_file("index.html", &body_size);

    int connectfd = getconnection(sockfd);

    //request handling
    char buf[1024];
    getrequest(connectfd, buf, sizeof(buf));

    printf("%s", buf);
    
    char* ch;
    do{
        ch = strchr(buf, '\r');
    }while(!ch);

    *ch = '\0';

    char** args = tokenise(buf);
    
    //printf("tokens\n");
    for(int i = 0; args[i] != NULL ;i++){
        printf("%s\n", args[i]);
    }


    char header[MAX_HEADER_SIZE]; 
    int header_len = create_header(header, MAX_HEADER_SIZE, body_size);

    ssize_t bytes_written = write_http(connectfd, header, header_len, body, body_size);

    printf("total bytes written: %zd\n", bytes_written);

    close(connectfd);
    close(sockfd);
    free_tokens(args);
    return 0;
}