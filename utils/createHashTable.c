/**
 * @file createHashTable.c
 * @version 1.0
 * @date 02/06/2021
 * @author Jonathan López Castellanos - Víctor Alfredo Barragán Paez
 * @title Programa createHashTable
 * @brief Programa de utilidad empleado para crear el archivo de la tabla hash y los datos de los registros procesados, a partir del archivo suministrado por Uber.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../types/record.h"

#define TRUE 1
#define FALSE 0

#define MAX_LINE_SIZE 150
#define MAX_SOURCE_ID 1160
#define MAX_ATTR_NEEDED 4
#define SOURCE_ID 0
#define DEST_ID 1
#define HOD 2
#define MEAN_TRAV_TIME 3

/**
 * @brief freeLinkedList Función auxiliar empleada para liberar la memoria reservada para cada cada lista enlazada de la tabla hash en memoria.
 * @param head Apuntador a un apuntador a la estructura record_t, la cabeza de la lista.
 */
void freeLinkedList ( record_t **head ) {
   record_t *tmp;

   while ( *head != NULL ) {
      tmp = *head;
      *head = ( *head )->next;
      free( tmp );
   }
}

/**
 * @brief addRecord Función empleada para añadir los datos de un registro en una lista enlazada de la tabla hash.
 * @param list Apuntador a un apuntador a la estructura record_t, la cabeza de la lista.
 * @param newRecord Variable de tipo record_t que contiene los datos del registro leído y a ser ingresados en la lista.
 */
void addRecord ( record_t **list, record_t newRecord ) {

   if ( *list == NULL ) {

      *list = ( record_t * ) malloc( sizeof( record_t ) );
      **list = newRecord;

   } else {

      record_t *aux = *list;

      while ( aux->next != NULL ) aux = aux->next;

      aux->next = ( record_t * ) malloc( sizeof( record_t ) );
      *aux->next = newRecord;
   }
}

/**
 * @brief main Función principal. Procesa el archivo suministrado por Uber y los almacena en un archivo binario, además de guardar las posiciones en dicho archivo del primer registro por id de origen (tabla hash).
 */
int main () {

   int totalNumRecords, attrRead, r;
   int writeProcDataErr = FALSE;

   FILE* fileIn;   
   FILE* fileOutProcessedData;
   FILE* fileOutHashTable;

   char line[ MAX_LINE_SIZE ];
   char* token;
   
   record_t * hashTable[ MAX_SOURCE_ID ] = { NULL };
   record_t currRecord; 
   record_t * temp = ( record_t * ) malloc( sizeof( record_t ) );
   recordRead_t toWrite;

   fileIn = fopen( "../data/unprocessedData.csv", "r" );
   fileOutProcessedData = fopen( "../data/processedData.bin", "wb" );
   fileOutHashTable = fopen( "../data/hashTable.bin", "wb" );

   if ( fileIn == NULL ) {
      printf( "Error al leer el archivo '../data/unprocessedData.bin'" );
      return -1;
   }

   // Read and ignore the line of column names
   fgets( line, MAX_LINE_SIZE, fileIn );

   while( TRUE ) {

      fgets( line, MAX_LINE_SIZE, fileIn );
      
      if ( feof( fileIn ) ) {
          break;
      }

      token = strtok( line, "," );

      attrRead = 0;

      while( token != NULL && attrRead < MAX_ATTR_NEEDED ) {

         switch ( attrRead ) {
            case SOURCE_ID:
               currRecord.sourceId = atoi( token ); 
               break;
            case DEST_ID:
               currRecord.destId = atoi( token );
               break;
            case HOD:
               currRecord.hourOfDay = atoi( token );
               break;
            case MEAN_TRAV_TIME:
               currRecord.meanTravelTime = atof( token );
               break;
         }

         currRecord.next = NULL;

         token = strtok( NULL, "," );

         ++attrRead;
      }

      addRecord( &hashTable[ currRecord.sourceId - 1 ], currRecord );
   }

   totalNumRecords = 0;

   for( int currSourceId = 1; currSourceId <= MAX_SOURCE_ID; ++currSourceId ) {
      
      temp = hashTable[ currSourceId - 1 ];
      
      r = fwrite( &totalNumRecords, sizeof(totalNumRecords), 1, fileOutHashTable );
      
      if ( r <= 0 ) {
         perror( "Error al escribir en el archivo '../data/hashTable.bin'" );
         break;
      }

      while( temp != NULL ) {

         toWrite.sourceId = temp->sourceId;
         toWrite.destId = temp->destId;
         toWrite.hourOfDay = temp->hourOfDay;
         toWrite.meanTravelTime = temp->meanTravelTime;

         r = fwrite( &toWrite, sizeof( toWrite ), 1, fileOutProcessedData );

         if ( r <= 0 ) {
            perror( "Error al escribir en el archivo '../data/processedData.bin'" );
            writeProcDataErr = TRUE;             
            break;
         }

         temp = temp->next;
         ++totalNumRecords;
      }

      if ( writeProcDataErr ) {
         break;
      }
   }

   for ( int i = 0; i < MAX_SOURCE_ID; ++i ) {
      freeLinkedList( &hashTable[ i ] );
      free( hashTable[ i ] ); 
   }
   
   free( temp );

   fclose( fileIn );
   fclose( fileOutProcessedData );
   fclose( fileOutHashTable );

   return 0;
}
