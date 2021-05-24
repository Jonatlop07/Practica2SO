#include "searchRecord.h"

float searchRecordMeanTravelTime( recordQuery_t *queryData  ) {
   int hash, notFound = FALSE;

   recordRead_t aux;

   FILE* fileInHashTable;
   FILE* fileInProcessedData;

   fileInHashTable = fopen( "./data/hashTable.bin", "rb" );
   fileInProcessedData = fopen( "./data/processedData.bin", "rb" );

   if ( fileInHashTable == NULL ) {
      printf( "Error al abrir el archivo './data/hashTable.bin'" );
      return -1;
   }

   if ( fileInProcessedData == NULL ) {
      printf( "Error al abrir el archivo './data/processedData.bin'" );
      return -1;
   }

   handleError( 
      fseek( fileInHashTable, ( queryData->sourceId - 1 ) * sizeof( int ), SEEK_SET ),
      "\n-->Error en fseek()"
   );

   handleError( fread( &hash, sizeof( hash ), 1, fileInHashTable ), "\n-->Error en fread()" );

   handleError( 
      fseek( fileInProcessedData, hash * sizeof( recordRead_t ), SEEK_SET ),
      "\n-->Error en fseek()"
   );

   // Busqueda del registro
   do {
      handleError( fread( &aux, sizeof( aux ), 1, fileInProcessedData ), "\n-->Error en fseek" );

      if ( queryData->sourceId != aux.sourceId || feof( fileInProcessedData ) ) {
         notFound = TRUE;
         break;
      }

   } while ( queryData->destId != aux.destId || queryData->hourOfDay != aux.hourOfDay );

   fclose( fileInHashTable );
   fclose( fileInProcessedData );

   return ( notFound ? 0 : aux.meanTravelTime );
}
