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
#define MAX_CLIENTS 2

pthread_t threads[ MAX_CLIENTS ];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t threadCondition = PTHREAD_COND_INITIALIZER; 

struct sockaddr_in clientInfo;

FILE *logFile;
int serverfd;

struct clientInfo {
   int *fd;
   struct sockaddr_in *socketInfo;
};

void signalInterruptHandler( int );
void * threadFunction( void * );
void * handleRequest( /*client_t*/int );

int main() {
   int opt = 1, threadIndex = 0;
   int clientfd;
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
         clientfd = accept( serverfd, ( struct sockaddr * ) &clientInfo, &socketSize ),
         "\n-->Error en accept()"
      );

      pthread_mutex_lock( &mutex );
      enqueue( clientfd );
      pthread_cond_signal( &threadCondition );
      pthread_mutex_unlock( &mutex );
   }
}

void signalInterruptHandler( int s ) {
   printf( "\nSe detecto la señal de interrupcion. Finalizando el programa...\n");

   for ( int i = 0; i < MAX_CLIENTS; ++i )
      handleError(
         pthread_cancel( threads[ i ] ),
         "\n-->Error en pthread_join()"
      );
   //pthread_mutex_destroy( &mutex );
   //handleError( pthread_cond_destroy( &threadCondition ), "\n-->Error en pthread_cond_destroy()" );
   close( serverfd );
   fclose( logFile );

   exit( 0 );
}

void * threadFunction( void * arg ) {
   /*while ( TRUE ) {
      client_t *client;
      int * client;
      pthread_mutex_lock( &mutex );

      if ( ( client = dequeue() ) == NULL ) {
         pthread_cond_wait( &threadCondition, &mutex );
         client = dequeue();
      }

      pthread_mutex_unlock( &mutex );     
      
      if ( client != NULL ) {
         perror( "Cliente" );
         handleRequest( client );
      }
   }*/
       while (TRUE)
    {
        int clientfd;
        pthread_mutex_lock(&mutex);
        if ((clientfd = dequeue()) == -1){
            pthread_cond_wait(&threadCondition,&mutex);
            clientfd = dequeue();
        }
        pthread_mutex_unlock(&mutex);
        if (clientfd != -1){
            printf("Entrando a handle con: %d \n", clientfd);
            handleRequest(clientfd);
        }else{
            sleep(1);
        }
    }

}

void * handleRequest( /*client_t *client*/int clientfd ) {
   //int clientfd = client->fd;
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
         inet_ntoa( clientInfo.sin_addr ),
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
   //free( client );
}
