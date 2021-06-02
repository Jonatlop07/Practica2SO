#include "clientQueue.h"

node_t *head;
node_t *tail;

void enqueue( client_t *data ) {
   node_t *newNode = malloc( sizeof( node_t ) );
   newNode->data = data;
   newNode->next = NULL;
   
   if ( head /*tail*/ == NULL ) {
      head = newNode;
      tail = newNode;
   } else {
      tail->next = newNode;
      tail = newNode;
   }
}

client_t* dequeue() {
   if ( head != NULL ) {
      client_t* result = head->data;

      node_t *aux = head;
      //head = head->next;

      if ( head == tail /*NULL*/ ) {
         head = NULL;
         tail = NULL;
      } else {
         head = head->next;
      }
      
      free( aux );

      return result;
   }
   return NULL;
}

