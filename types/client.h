#ifndef CLIENT_H_
#define CLIENT_H_

#include <netinet/in.h>

typedef struct client {
   int fd;
   struct sockaddr_in socketInfo;
} client_t;

#endif
