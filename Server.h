#include <stdlib.h>

int setup_socket();
int getconnection(int sockfd);
ssize_t write_http(int connection_fd, char* header, size_t header_size, char* body, size_t body_size);
ssize_t getrequest(int connection_fd, char* buf, size_t count);