#include "HTTP.h"
#include "Tokeniser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>

typedef struct {
    int code;
    const char* resp;
    size_t len;
} ErrResp;

#define ERR_BODYLESS(code, phrase) \
    "HTTP/1.1 " #code " " phrase "\r\n" \
    "Content-Length: 0\r\n" \
    "Connection: close\r\n" \
    "\r\n"

static const char RESP_400[] = ERR_BODYLESS(400, "Bad Request");
static const char RESP_403[] = ERR_BODYLESS(403, "Forbidden");
static const char RESP_404[] = ERR_BODYLESS(404, "Not Found");
static const char RESP_405[] =
    "HTTP/1.1 405 Method Not Allowed\r\n"
    "Allow: GET\r\n"
    "Content-Length: 0\r\n"
    "Connection: close\r\n"
    "\r\n";
static const char RESP_414[] = ERR_BODYLESS(414, "URI Too Long");
static const char RESP_431[] = ERR_BODYLESS(431, "Request Header Fields Too Large");
static const char RESP_500[] = ERR_BODYLESS(500, "Internal Server Error");
static const char RESP_501[] = ERR_BODYLESS(501, "Not Implemented");
static const char RESP_505[] = ERR_BODYLESS(505, "HTTP Version Not Supported");

#define ERR_ENTRY(code, str) { code, str, sizeof(str) - 1 }

static const ErrResp err_table[] = {
    ERR_ENTRY(400, RESP_400),
    ERR_ENTRY(403, RESP_403),
    ERR_ENTRY(404, RESP_404),
    ERR_ENTRY(405, RESP_405),
    ERR_ENTRY(414, RESP_414),
    ERR_ENTRY(431, RESP_431),
    ERR_ENTRY(500, RESP_500),
    ERR_ENTRY(501, RESP_501),
    ERR_ENTRY(505, RESP_505),
};

// Takes the positive code (e.g. 404). Unknown codes fall back to 500.
static const ErrResp* find_err_resp(int code){
    size_t n = sizeof(err_table) / sizeof(err_table[0]);
    for(size_t i = 0; i < n; i++){
        if(err_table[i].code == code){
            return &err_table[i];
        }
    }
    return &err_table[6]; // the 500 entry
}

ssize_t send_error(int fd, int code){
    const ErrResp* e = find_err_resp(code);
    return write(fd, e->resp, e->len);
}

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
    { ".jpg",  JPEG },
    { ".jpeg", JPEG },
    { ".webp", WEBP },
    { ".svg",  SVG  },
    { ".gif",  GIF  },
    { ".json", JSON },
    { ".xml",  XML  },
    { ".pdf",  PDF  },
    { ".mp4",  MP4  },
    { ".webm", WEBM },
    { ".mp3",  MP3  }
};


char** HTTP_getargs(char* buf){

    char* ch;
    //do{
    ch = strchr(buf, '\r');
    //}while(!ch);
    if(!ch) return NULL;
    *ch = '\0';

    char** args = tokenise(buf);
   
    return args;
}

int parse_path(char* path, size_t path_size, char* default_file){
    if(strstr(path, "..") != NULL){//we found an occurance of .. , refuse.
        return 400;// this could only occur on curl, not browser side.
    }

    if(strcmp(path, "www/") == 0){//no field passed. default file.
        if(strlen(path) + strlen(default_file) >= path_size){
            return 1;   // wouldn't fit
        }
        strcat(path, default_file);
    }

    return 0;
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
            *error_out = 200;
            return ext_table[i].type;
        }
    }

    return ERROR;
}

int create_header(char* msg, size_t msg_size, long size, char* path){

    int status = 0;
    FileType ft = getFileExt(path, &status);
    
    char* content;
    
    switch(ft){
        case HTML:  content = "text/html";        break;
        case CSS:   content = "text/css";         break;
        case JS:    content = "text/javascript";  break;
        case JPEG:  content = "image/jpeg";       break;
        case PNG:   content = "image/png";        break;
        case WEBP:  content = "image/webp";       break;
        case SVG:   content = "image/svg+xml";    break;
        case GIF:   content = "image/gif";        break;
        case JSON:  content = "application/json"; break;
        case XML:   content = "application/xml";  break;
        case PDF:   content = "application/pdf";  break;
        case MP4:   content = "video/mp4";        break;
        case WEBM:  content = "video/webm";       break;
        case MP3:   content = "audio/mpeg";       break;
        default:    content = "application/octet-stream"; break;
    }

    int len = snprintf(msg, msg_size,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "\r\n", content,size);

    if(len >= msg_size) return -500;

    return len;
}