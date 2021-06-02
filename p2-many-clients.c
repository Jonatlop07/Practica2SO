/**
 * @file p2-many-clients.c
 * @version 1.0
 * @date 02/06/2021
 * @author Jonathan López Castellanos - Víctor Alfredo Barragán Paez
 * @title Programa de varios clientes
 * @brief Programa de utilidad que realiza la prueba de conexión de varios clientes con el servidor y solicitudes con datos aleatorios.
 */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "./utils/errorHandler.h"
#include "./types/record.h"

#define PORT 3535
#define NUM_THREADS 65

/**
 * @brief createClient Función que ejecutan los hilos. Se encarga de establecer una conexión entre un nuevo cliente y el servidor y de realizar solicitudes con datos aleatorios hasta que el id de origen que envía el cliente sea 0, lo cual permite a ambas partes finalizar la conexión y desocupar el socket del cliente.
 */
void * createClient();

/**
 * @brief main Función principal. Se encarga de crear los hilos que ejecutan la función createClient, así como de esperar a que estos terminen finalicen su ejecución.
 */
int main() {
   int *rh0;
   pthread_t clientThreads[ NUM_THREADS ];

   for ( int i = 0; i < NUM_THREADS; ++i )
      handleError( 
         pthread_create( &clientThreads[ i ], NULL, ( void * ) createClient, NULL ),
         "\n-->Error en pthread_create()"
      );

   for ( int i = 0; i < NUM_THREADS; ++i )
      handleError( 
         pthread_join( clientThreads[ i ], ( void ** ) &rh0 ),
         "\n-->Error en pthread_join()"
      );

   printf( "\n" );

}

void * createClient() {
   float res = 0.0;
   recordQuery_t queryParams = { -1, 0, 0 };
   struct sockaddr_in client;
   int clientfd;

   handleError( clientfd = socket( AF_INET, SOCK_STREAM, 0 ), "\n-->Error en socket()" );

   client.sin_family = AF_INET;
   client.sin_port = htons( PORT );
   client.sin_addr.s_addr = INADDR_ANY;

   handleError( 
      connect( clientfd, ( struct sockaddr * ) &client, ( socklen_t ) sizeof( struct sockaddr ) ), 
      "\n-->Error en connect()"
   );

   while ( queryParams.sourceId != 0 ) {
      queryParams.sourceId = rand() % 1159;
      queryParams.destId = rand() % 1159;
      queryParams.hourOfDay = rand() % 23;

      handleError( send( clientfd, &queryParams, sizeof( queryParams ), 0 ), "\n-->Error en send():" );

      if ( recv( clientfd, &res, sizeof( res ), 0 ) < sizeof( res ) )
         break;
      else
         printf(
            "\nCliente: %d, consulta(%d - %d - %d), resultado(%.2f)",
            clientfd, queryParams.sourceId, queryParams.destId, queryParams.hourOfDay, res
         );
   }  

   close( clientfd );
   pthread_exit( NULL );
}

