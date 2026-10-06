#include "HTTP.h"
#include "Tokeniser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

typedef enum fileType_e{

    HTML,
    JS,
    CSS,

    JPEG,
    PNG,
    WEBP,
    SVG,
    GIF,

    JSON,
    XML,

    PDF,

    MP4,
    WEBM,
    MP3,

    ERROR
}FileType;

typedef struct{
    const char* ext;
    FileType type;
}ExtEntry;

static const ExtEntry ext_table[] = {
    { ".html", HTML },
    { ".htm",  HTML },
    { ".css",  CSS  },
    { ".js",   JS   },
    { ".png",  PNG  },
    { ".jpg",  JPEG  },
    { ".jpeg", JPEG  },
    { ".json", JSON }
};

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

static FileType getFileExt(char* path, int* error_out){
    char* lastdot = strrchr(path, '.');
    if(lastdot == NULL){
        *error_out = 404;
        return ERROR;
    }

   // size_t length = strlen(lastdot);
    //printf("length %zu\n", length);

    for(size_t i = 0; i < sizeof(ext_table) / sizeof(ext_table[0]); i++){
        if (strcmp(lastdot, ext_table[i].ext) == 0) {
            return ext_table[i].type;
        }
    }

    return ERROR;
}

int create_header(char* msg, size_t msg_size, long size, char* path){

    int i = 0;
    FileType ft = getFileExt(path, &i);
    
    char* content;
    
    switch(ft){
        case HTML:
            content = "text/html";
            break;
        case JSON:
            content = "application/json";
            break;
         default:   
            content = "application/octet-stream"; 
            break;
    }

    int len = snprintf(msg, msg_size,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "\r\n", content,size);

    return len;
}



