#ifndef CLIENTQUEUE_H_
#define CLIENTQUEUE_H_

#include <stdlib.h>
#include "../types/client.h"

typedef struct node {
   client_t *data;
   struct node *next;
} node_t;

void enqueue( client_t* );
client_t* dequeue();

#endif
