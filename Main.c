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
    int connectfd = *(int*)arg;
    free(arg);

    char buf[1024];
    char** args = NULL;
    char* body = NULL;
    int status = 0;                       // negative = error to send

    if(getrequest(connectfd, buf, sizeof(buf)) <= 0) goto done;  // nothing to reply to

    args = HTTP_getargs(buf);
    if(!args || !args[0] || !args[1]){ status = -400; goto done; }

    if(strcmp(args[0], "GET") != 0){ status = -405; goto done; }

    char path[PATH_MAX];
    snprintf(path, sizeof(path), "www%s", args[1]);

    if(parse_path(path, "index.html") != 0){ status = -403; goto done; }

    long body_size;
    body = readin_file(path, &body_size);
    if(!body){ status = -404; goto done; }

    char header[MAX_HEADER_SIZE];
    int header_len = create_header(header, MAX_HEADER_SIZE, body_size, path);
    if(header_len < 0 || header_len >= MAX_HEADER_SIZE){ status = -500; goto done; }

    write_http(connectfd, header, header_len, body, body_size);

done:
    if(status < 0) send_error(connectfd, -status);
    free(body);
    if(args) free_tokens(args);
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