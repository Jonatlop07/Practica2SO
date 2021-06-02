/**
 * @file p2-server.c
 * @version 1.0
 * @date 02/06/2021
 * @author Jonathan López Castellanos - Víctor Alfredo Barragán Paez
 * @title Programa del servidor
 * @brief Programa que crea un socker que actúa como servidor y administra un máximo de 32 conexiones con clientes al mismo tiempo, además de procesar las solicitudes de estos.
 */
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <pthread.h>
#include "./dataStructures/clientQueue.h"
#include "./search/searchRecord.h"

#define PORT 3535
#define BACKLOG 100
#define MAX_CLIENTS 32

/**
 * @brief Arreglo de hilos pthread_t cuyo tamaño es el máximo de clientes que pueden establecer una conexión con el servidor al mismo tiempo.
 */
pthread_t threads[ MAX_CLIENTS ];

/**
 * @brief Mutex empleado para evitar que los hilos y desencolen de la cola de clientes o escriban en el archivo log al mismo tiempo
 */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Condición de hilo empleada para bloquear el desencolamiento hasta que se encole un nuevo cliente
 */
pthread_cond_t threadCondition = PTHREAD_COND_INITIALIZER; 

FILE *logFile;
int serverfd;

/**
 * @brief signalInterruptHandler Función empleada para cerrar recursos una vez se envía la señal de interrupción control + C
 */
void signalInterruptHandler( int );

/**
 * @brief threadFunction Función que ejecuta cada hilo una vez es creado. Se encarga de desencolar clientes de la cola y hacer el llamado a la función que maneja la petición del cliente, siempre que exista en la cola.
 */
void * threadFunction( void * );

/**
 * @brief handleRequest Función que maneja la petición del cliente, recibe los datos de consulta y llama a la función encargada de buscar el registro solicitado y devolver el tiempo medio de viajes.
 * @param Apuntador a la estructura client_t con los datos de conexión del cliente.
 */
void * handleRequest( client_t* );

/**
 * @brief main Función principal. Crea un socket de servidor, crea los hilos, acepta las conexiones entrantes y encola los clientes que se conectan al servidor.
 */
int main() {
   int opt = 1, threadIndex = 0;
   struct sigaction signalIntHandler;

   struct sockaddr_in server;
   socklen_t socketSize;
  
   // logFile es un apuntador a FILE de ambito global 
   logFile = fopen( "./output/log.out", "w" );

   if ( logFile == NULL ) {
      printf( "\nError creando archivo log" );
      exit( -1 );
   }

   // Codigo para manejar el comportamiento de la interrupcion
   // ctrl + C (liberacion de recursos)
   signalIntHandler.sa_handler = signalInterruptHandler;
   sigemptyset( &signalIntHandler.sa_mask );
   signalIntHandler.sa_flags = 0;
   sigaction( SIGINT, &signalIntHandler, NULL );
   // fin
   while ( threadIndex < MAX_CLIENTS )
      pthread_create( &threads[ threadIndex++ ], NULL, threadFunction, NULL );

   handleError( serverfd = socket( AF_INET, SOCK_STREAM, 0 ), "\n-->Error en socket()" ); 

   // Configuración del servidor
   server.sin_family = AF_INET;
   server.sin_port = htons( PORT );
   server.sin_addr.s_addr = INADDR_ANY;
   bzero( server.sin_zero, 8 );

   setsockopt( serverfd, SOL_SOCKET, SO_REUSEADDR, ( char * ) &opt, sizeof( opt ) );
   
   handleError( 
      bind( serverfd, ( struct sockaddr * ) &server, sizeof( struct sockaddr ) ), 
      "\n-->Error en bind()"
   ); 
   handleError( listen( serverfd, BACKLOG ), "\n-->Error en listen()" );

   while ( TRUE ) {
      client_t *client = malloc( sizeof ( client_t ) );
      handleError( 
         client->fd = accept( serverfd, ( struct sockaddr * ) &client->socketInfo, &socketSize ),
         "\n-->Error en accept()"
      );
      pthread_mutex_lock( &mutex );
      enqueue( client );
      pthread_cond_signal( &threadCondition );
      pthread_mutex_unlock( &mutex );
   }
}

void signalInterruptHandler( int s ) {
   printf( "\nSe detecto la señal de interrupcion. Finalizando el programa...\n");
   for ( int i = 0; i < MAX_CLIENTS; ++i )
      handleError( pthread_cancel( threads[ i ] ), "\n-->Error en pthread_join()" );
   handleError( pthread_mutex_destroy( &mutex ), "\n-->Error en pthread_mutex_destroy()" );
   handleError( pthread_cond_destroy( &threadCondition ), "\n-->Error en pthread_cond_destroy()" );
   close( serverfd );
   fclose( logFile );
   exit( 0 );
}

void * threadFunction( void * arg ) {
   while ( TRUE ) {
      client_t *client;
      pthread_mutex_lock( &mutex );

      if ( ( client = dequeue() ) == NULL ) {
         pthread_cond_wait( &threadCondition, &mutex );
         client = dequeue();
      }

      pthread_mutex_unlock( &mutex );     

      if ( client != NULL )
         handleRequest( client );
   }
}

void * handleRequest( client_t *client ) {
   int clientfd = client->fd;
   float errorCode = -1;

   while ( TRUE ) {
      recordQuery_t clientQuery;

      if ( recv( clientfd, &clientQuery, sizeof( clientQuery ), 0 ) < sizeof( clientQuery ) ) {
         send( clientfd, &errorCode, sizeof( errorCode ), 0 );
         continue;
      }

      if ( clientQuery.sourceId == 0 ) break;
      
      time_t _time = time( 0 );
      struct tm* localTime = localtime( &_time );

      // Usar mutex para evitar que todos los
      // hilos escriban en el archivo al mismo tiempo
      pthread_mutex_lock( &mutex );
      fprintf(
         logFile,
         "Fecha [%04d%02d%02d%02d%02d%02d] Cliente [%s] [busqueda - %d - %d - %d]\n",
         localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
         localTime->tm_hour, localTime->tm_min, localTime->tm_sec,
         inet_ntoa( client->socketInfo.sin_addr ),
         clientQuery.sourceId,
         clientQuery.destId,
         clientQuery.hourOfDay
      );
      pthread_mutex_unlock( &mutex );

      // Esta funcion es importada del archivo p2-searchRecord.h
      float result = searchRecordMeanTravelTime( &clientQuery );
      send( clientfd, &result, sizeof( result ), 0 ); 
   }

   close( clientfd );
   free( client );
}
