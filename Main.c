#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Server.h"
#include "FileHandler.h"
#include "Tokeniser.h"
#include "HTTP.h"

#include <unistd.h>
#include <linux/limits.h>

#define MAX_HEADER_SIZE 256

static void free_all(int connectfd, int sockfd,char* body, char** args){
    close(connectfd);
    close(sockfd);
    free(body);
    free_tokens(args);
}

int main(){

    //socket setup
    int sockfd = setup_socket();

    int connectfd = getconnection(sockfd);

    //request handling
    char buf[1024];
    getrequest(connectfd, buf, sizeof(buf));

    printf("%s", buf);
    
    char** args = HTTP_getargs(buf);
    
    /*printf("tokens\n");
    for(int i = 0; args[i] != NULL ;i++){
        printf("%s\n", args[i]);
    }*/

    char path[PATH_MAX];
    snprintf(path, sizeof(path), "www%s", args[1]);
    int valid_path = parse_path(path, "index.html");
    if(valid_path != 0){
        printf("malicious attempt\n");
        return 1;
    }


    //printf("%s\n",path);

    long body_size;
    char* body = readin_file(path, &body_size);

    char header[MAX_HEADER_SIZE]; 
    int header_len = create_header(header, MAX_HEADER_SIZE, body_size);

    ssize_t bytes_written = write_http(connectfd, header, header_len, body, body_size);

    printf("total bytes written: %zd\n", bytes_written);


    free_all(connectfd, sockfd,body, args);
    return 0;
}