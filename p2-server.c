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
#define MAX_CLIENTS 3

pthread_mutex_t mutex;

pthread_t threads[ MAX_CLIENTS ];

FILE *logFile;
int serverfd;

struct clientInfo {
   int *fd;
   struct sockaddr_in *socketInfo;
};

void signalInterruptHandler( int );
void * threadFunction( void * );
void * handleRequest( void * );


int main() {
   int opt = 1, threadIndex = 0;
   struct sigaction signalIntHandler;

   struct sockaddr_in server;
   socklen_t socketSize;
   
   client_t client; 

   pthread_t threadIds[ MAX_CLIENTS ];
  
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

   serverfd = socket( AF_INET, SOCK_STREAM, 0 );

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
   handleError( listen( serverfd, MAX_CLIENTS ), "\n-->Error en listen()" );

   // while ( threadIndex < MAX_CLIENTS )
      // pthread_create( &threads[ threadIndex++ ], NULL, threadFunction, NULL );

   pthread_mutex_init( &mutex, NULL );

   while ( TRUE ) {
      socketSize = sizeof( struct sockaddr_in );

      handleError( 
         client.fd = accept( serverfd, ( struct sockaddr * ) client.socketInfo, &socketSize ),
         "\n-->Error en accept()"
      );

      /*pthread_mutex_lock( &mutex );
      enqueue( &client );
      pthread_mutex_unlock( &mutex );*/

      handleError(
         pthread_create( &threadIds[ threadIndex++ ], NULL, handleRequest, ( void * ) &client ),
         "\n-->Error en pthread_create()" 
      );

      if ( threadIndex >= MAX_CLIENTS ) {
         threadIndex = 0;
         
         while ( threadIndex < MAX_CLIENTS )
            handleError( pthread_join( threadIds[ threadIndex++ ], NULL ), "\n-->Error en pthread_join()" );

         threadIndex = 0;
      }
   }
}

void signalInterruptHandler( int s ) {
   printf( "\nSe detecto la señal de interrupcion. Finalizando el programa...\n");

   pthread_mutex_destroy( &mutex );
   close( serverfd );
   fclose( logFile );

   exit( 0 );
}

void * threadFunction( void * arg ) {
   while ( TRUE ) {
      client_t *client = dequeue();      
      
      if ( client != NULL ){
         handleRequest( client );  
      }
   }

}

void * handleRequest( void *data ) {
   client_t *client = data;
   free( data );

   int clientfd = client->fd;
   float errorCode = -1;

   recordQuery_t clientQuery;

   while ( TRUE ) {
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
         inet_ntoa( client->socketInfo->sin_addr ),
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
}
