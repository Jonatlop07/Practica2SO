#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include "./record.h"
 
#define PORT 3535

#define ORIGIN_INPUT 1
#define DEST_INPUT 2
#define HOUR_INPUT 3
#define SEND_REQUEST 4
#define EXIT 5

void handleError( char* msg ) {
   perror( msg );
   exit( EXIT_FAILURE );
}

int sendRequest( int clientfd, recordQuery_t queryParams ) {
   if ( send( clientfd, &queryParams, sizeof( queryParams ), 0 ) < 0 )
      handleError( "\n-->Error en send():" );
   
   int res = 2;

   recv( clientfd, &res, sizeof( res ), 0 );
   printf( "%d\n", res );
   // Enviar datos al servidor
   // Recibir respuesta
   
   return 0;
}

int main( int argc, char *argv[] ) {
   int clientfd, r;
   int option;
   recordQuery_t queryParams;

   struct sockaddr_in client;
   struct hostent *he;

   char buffer[ 32 ];

   clientfd = socket( AF_INET, SOCK_STREAM, 0 );

   if ( clientfd < 0 )
      handleError( "\n-->Error en socket()" );
   
   client.sin_family = AF_INET;
   client.sin_port = htons( PORT );
   inet_aton( argv[ 1 ], &client.sin_addr );
   
   if ( connect( clientfd, ( struct sockaddr * ) &client, ( socklen_t ) sizeof( struct sockaddr ) ) < 0 )
      handleError( "\n-->Error en connect()" );
   
   do {
      system( "clear" );
      printf( "Bienvenido (UwU)\n\n" );
      printf( "Menu:\n");
      printf( "1. Ingresar origen\n" );
      printf( "2. Ingresar destino\n" );
      printf( "3. Ingresar hora\n" );
      printf( "4. Buscar tiempo de viaje medio\n" );
      printf( "5. Finalizar\n" );
      printf( "\nIngrese el numero de la opcion deseada:  " );
      scanf( "%i", &option );
      
      switch ( option ) {
         case ORIGIN_INPUT:
	         system( "clear" );
	         printf( "\nIngrese ID del origen:  " );
	         scanf( "%i", &queryParams.sourceId );
		      break;
	      case DEST_INPUT:
	         system( "clear" );
            printf( "\nIngrese ID del destino:  " );
	         scanf( "%i", &queryParams.destId );
            break;
	      case HOUR_INPUT:
	         system( "clear" );
		      printf( "\nIngrese hora del dia:  " );
	         scanf( "%i", &queryParams.hourOfDay );
		      break;
	      case SEND_REQUEST:
	         system( "clear" );
		      clock_t begin, end;
		      begin = clock();
            
            sendRequest( clientfd, queryParams );
            // Recivir datos del servidor
            //r = recv( clientfd, buffer, 32, 0);

		      end = clock();

		      double duration = ( double ) ( end - begin ) / CLOCKS_PER_SEC;

		      printf( "\n\nLa busqueda tomo %2.6f segundos.\n", duration );
	         printf( "\nPresione Enter para continuar" );

	         getchar();
	         getchar();
	         break;
	      case EXIT:
            // Enviar opcion de finalizacion al servidor, cerrar socket de cliente
            queryParams.sourceId = 0;
            if ( send( clientfd, &queryParams, sizeof( queryParams ), 0 ) < 0 )
               handleError( "\n-->Error en send()" );

            close( clientfd );
		      printf( "\nHasta luego (UwU)\n" );
	         exit ( 0 );
	         break;
         default:
	         printf( "\nError, vuelva a digitar la opcion\n" );
	         getchar();
	         getchar();
      }
   } while ( 1 );
}
