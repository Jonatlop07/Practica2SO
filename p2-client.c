/**
 * @file p2-client.c
 * @version 1.0
 * @date 02/06/2021
 * @author Jonathan López Castellanos - Víctor Alfredo Barragán Paez
 * @title Programa del cliente
 * @brief Programa que crea una conexión con el servidor y presenta una interfaz de consola para el ingreso de los datos del registro a buscar.
 */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include "./utils/errorHandler.h"
#include "./types/record.h"
 
#define TRUE 1
#define PORT 3535

#define ORIGIN_INPUT 1
#define DEST_INPUT 2
#define HOUR_INPUT 3
#define SEND_REQUEST 4
#define EXIT 5

/**
 * @brief sendRequest Función que envía la petición al servidor, con los datos de consulta, y retorna la respuesta.
 * @param clientfd Descriptor del socket del cliente
 * @param queryParams Variable de tipo estructura que almacena los datos de consulta del cliente.
 * @return Respuesta del servidor (media de tiempos de viaje), -1 si ocurrió un error, 0 si el registro no se encontró.
 */
int sendRequest( int clientfd, recordQuery_t queryParams ) {
   float res = 0.0;

   handleError( send( clientfd, &queryParams, sizeof( queryParams ), 0 ), "\n-->Error en send():" );
   
   if ( recv( clientfd, &res, sizeof( res ), 0 ) < sizeof( res ) ) return -1;
   return res;
}

/**
 * @brief main Función principal. Crea un socket de cliente, realiza la conexión con el servidor y administra el ingreso y envío de los datos de consulta junto con la finalización de la conexión.
 * @param argv El primer argumento de la entrada por terminal es la dirección IP del cliente que realizará la conexión con el servidor
 */
int main( int argc, char *argv[] ) {
   int clientfd, r, option;
   recordQuery_t queryParams = { -1, 0, 0 };

   struct sockaddr_in client;
   struct hostent *he;

   handleError( clientfd = socket( AF_INET, SOCK_STREAM, 0 ), "\n-->Error en socket()" );
  
   // Configuracion inicial del cliente 
   client.sin_family = AF_INET;
   client.sin_port = htons( PORT );

   if ( argv[ 1 ] == NULL ) {
      printf( "\nError: No se ha especificado la direccion IP del cliente\n" );
      close( clientfd );
      exit( -1 );
   }

   inet_aton( argv[ 1 ], &client.sin_addr );
   
   handleError( 
      connect( clientfd, ( struct sockaddr * ) &client, ( socklen_t ) sizeof( struct sockaddr ) ), 
      "\n-->Error en connect()"
   );
   
   do {
      system( "clear" );
      printf( "Bienvenido \\(^u^)/\n\n" );
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
            
            float response = sendRequest( clientfd, queryParams );
            
            end = clock();

            if ( response >= 0 ) {
               if ( response == 0 )
                  printf( "\n\nNA\n" );
               else
                  printf( "\n\nTiempo de viaje medio: %.3f\n", response );

               double duration = ( double ) ( end - begin ) / CLOCKS_PER_SEC;
               printf( "\n\nLa busqueda tomo %2.6f segundos.\n", duration );
            } else
               printf( "\n\nError al procesar la solicitud. Por favor intentelo de nuevo.\n");
            break;
         case EXIT:
            // Enviar opcion de finalizacion al servidor, cerrar socket de cliente
            queryParams.sourceId = 0;
            handleError( send( clientfd, &queryParams, sizeof( queryParams ), 0 ), "\n-->Error en send()" );
            
            close( clientfd );
            printf( "\nHasta luego, que tenga un buen dia :D\n\n" );
            exit ( 0 );
            break;
         default:
            printf( "\nError, vuelva a digitar la opcion\n" );
      }

      if ( option >= ORIGIN_INPUT && option < EXIT && option != SEND_REQUEST )
         printf( "\nSe ha recibido el dato\n" );
      printf( "\n\nPresione una tecla para continuar" );
      getchar();
      getchar();
   } while ( TRUE );
}
