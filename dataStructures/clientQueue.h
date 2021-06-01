#ifndef CLIENTQUEUE_H_
#define CLIENTQUEUE_H_

#include <stdlib.h>
#include <stdio.h>
#include "../types/client.h"

typedef struct node {
   //client_t *data;
   int data;
   struct node *next;
} node_t;

void enqueue( /*client_t**/int );
/*client_t**/int dequeue();

#endif
