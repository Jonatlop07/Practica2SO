typedef struct record {
   int sourceId;
   int destId;
   int hourOfDay;
   float meanTravelTime;
   struct record *next;
} record_t;

typedef struct recordLinkedList {
   record_t * head;
} recordList_t;

typedef struct recordRead {
   int sourceId;
   int destId;
   int hourOfDay;
   float meanTravelTime;
} recordRead_t;

typedef struct recordQuery {
   int sourceId;
   int destId;
   int hourOfDay;
} recordQuery_t;
