#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Server.h"
#include "FileHandler.h"
#include "Tokeniser.h"
#include "HTTP.h"

#include <unistd.h>
#include <linux/limits.h>
#include <pthread.h>

#define MAX_HEADER_SIZE 256

static void cleanup(int connectfd, int sockfd,char* body, char** args){
    close(connectfd);
    close(sockfd);
    free(body);
    free_tokens(args);
}

static void* work(void* arg){
    //int* confd = (int*)arg;
    //int connectfd = *(confd);
    int connectfd = *(int*)arg;
    free(arg);

    //request handling
    char buf[1024];
    if(getrequest(connectfd, buf, sizeof(buf)) <= 0) goto done;

    printf("%s", buf);
    
    char** args = NULL;

    args = HTTP_getargs(buf);
    if (!args || !args[0] || !args[1]) goto done;

   // HTTP_getargs(buf);
    
    /*printf("tokens\n");
    for(int i = 0; args[i] != NULL ;i++){
        printf("%s\n", args[i]);
    }*/

    char path[PATH_MAX];
    snprintf(path, sizeof(path), "www%s", args[1]);
    
    int valid_path = parse_path(path, "index.html");
    if(valid_path != 0){
        printf("malicious attempt\n");
        goto done;
    }


    //printf("%s\n",path);

    long body_size;
    char* body = readin_file(path, &body_size);

    if(!body) goto done;

    char header[MAX_HEADER_SIZE]; 
    int header_len = create_header(header, MAX_HEADER_SIZE, body_size, path);

    ssize_t bytes_written = write_http(connectfd, header, header_len, body, body_size);

    printf("total bytes written: %zd\n", bytes_written);

done:
    free(body);          
    if (args) free_tokens(args);
    close(connectfd);
    return NULL;
}

int main(){

    //socket setup
    int sockfd = setup_socket();

    for(;;){
        int connectfd = getconnection(sockfd);
        int* arg = malloc(sizeof *arg);
        *arg = connectfd;
        pthread_t thread;

        if (pthread_create(&thread, NULL, work, arg) != 0) {
            free(arg);
            close(connectfd);
            continue;
        }

        pthread_detach(thread);

        //cleanup(connectfd, sockfd,body, args);
    }
    close(sockfd);
    return 0;
}