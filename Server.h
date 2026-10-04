#include <stdlib.h>

int setup_socket();
int getconnection(int sockfd);
int create_header(char* msg, size_t msg_size, long size);
ssize_t write_http(int connection_fd, char* header, size_t header_size, char* body, size_t body_size);
ssize_t getrequest(int connection_fd, char* buf, size_t count);