#include "HTTP.h"
#include "Tokeniser.h"
#include <stdlib.h>
#include <string.h>

char** HTTP_getargs(char* buf){

    char* ch;
    do{
        ch = strchr(buf, '\r');
    }while(!ch);

    *ch = '\0';

    char** args = tokenise(buf);
   
    return args;
}

int parse_path(char* path, char* default_file){
    if(strstr(path, "..") != NULL){//we found an occurance of .. , refuse.
        return 1;// this could only occur on curl, not browser side.
    }

    if(strcmp(path, "www/") == 0){//no field passed. default file.
        strcat(path, default_file);
        return 0;
    }else{

        return 0;
    }
}

