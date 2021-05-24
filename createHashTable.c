#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "record.h"

#define TRUE 1
#define FALSE 0

#define MAX_LINE_SIZE 150
#define MAX_SOURCE_ID 1160
#define MAX_ATTR_NEEDED 4
#define SOURCE_ID 0
#define DEST_ID 1
#define HOD 2
#define MEAN_TRAV_TIME 3

void freeLinkedList ( record_t **head ) {
   record_t *tmp;

   while ( *head != NULL ) {
      tmp = *head;
      *head = ( *head )->next;
      free( tmp );
   }
}

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

   fileIn = fopen( "./unprocessedData.csv", "r" );
   fileOutProcessedData = fopen( "./processedData.bin", "wb" );
   fileOutHashTable = fopen( "./hashTable.bin", "wb" );

   if ( fileIn == NULL ) {
      printf( "Error al leer el archivo 'unprocessedData'" );
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
         perror( "Error al escribir en el archivo 'hashTable.bin'" );
         break;
      }

      while( temp != NULL ) {

         toWrite.sourceId = temp->sourceId;
         toWrite.destId = temp->destId;
         toWrite.hourOfDay = temp->hourOfDay;
         toWrite.meanTravelTime = temp->meanTravelTime;

         r = fwrite( &toWrite, sizeof( toWrite ), 1, fileOutProcessedData );

         if ( r <= 0 ) {
            perror( "Error al escribir en el archivo 'processedData.bin'" );
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
