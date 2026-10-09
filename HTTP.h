#include <stddef.h>
#include <sys/types.h>

char** HTTP_getargs(char* buf);
int parse_path(char* path, char* default_file);
int create_header(char* msg, size_t msg_size, long size, char* path);
ssize_t send_error(int fd, int code);