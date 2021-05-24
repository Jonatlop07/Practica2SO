#include "errorHandler.h"

void handleError( int returnCode, char* msg ) {
   if ( returnCode < 0 ) {
      perror( msg );
      exit( EXIT_FAILURE );
   }
}
