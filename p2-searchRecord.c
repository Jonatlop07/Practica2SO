#include "p2-searchRecord.h"

float searchRecordMeanTravelTime( recordQuery_t *queryData  ) {
   FILE* fileInHashTable;
   FILE* fileInProcessedData;
   
   recordRead_t aux;
   
   int r, hash;
   int notFound = FALSE;

   fileInHashTable = fopen( "./hashTable.bin", "rb" );
   fileInProcessedData = fopen( "./processedData.bin", "rb" );

   if ( fileInHashTable == NULL ) {
      printf( "Error al leer el archivo 'hashTable.bin'" );
      return -1;
   }

   if ( fileInProcessedData == NULL ) {
      printf( "Error al leer el archivo 'processedData.bin'" );
      return -1;
   }

   r = fseek( fileInHashTable, ( queryData->sourceId - 1 ) * sizeof( int ), SEEK_SET );

   if ( r < 0 ) {
      perror( "Error al buscar una determinada posición en el archivo 'hashTable.bin'" );
      return -1;
   }

   r = fread( &hash, sizeof( hash ), 1, fileInHashTable );
   
   if ( r <= 0 ) {
      perror( "Error al leer en el archivo 'hashTable.bin'" );
      return -1;
   }

   r = fseek( fileInProcessedData, hash * sizeof( recordRead_t ), SEEK_SET );

   if ( r < 0 ) {
      perror( "Error al buscar una determinada posición en el archivo 'processedData.bin'" );
      return -1;
   }

   //search algorithm
   do {
      r = fread( &aux, sizeof( aux ), 1, fileInProcessedData );

      if ( r <= 0 ) {
         perror( "Error al leer en el archivo 'processedData.bin'" );
         return -1;
      }

      if ( queryData->sourceId != aux.sourceId || feof( fileInProcessedData ) ) {
         notFound = TRUE;
         break;
      }

   } while ( queryData->destId != aux.destId || queryData->hourOfDay != aux.hourOfDay );

   fclose( fileInHashTable );
   fclose( fileInProcessedData );

   return ( notFound ? -1 : aux.meanTravelTime );
}
