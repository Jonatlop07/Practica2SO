/**
 * @file record.h
 * @version 1.0
 * @date 02/06/2021
 * @author Jonathan López Castellanos - Víctor Alfredo Barragán Paez
 * @title Estructuras auxiliares para los registros
 * @brief Estructuras empleadas para representar la información de los registros suministrados por el archivo de Uber.
 */

/**
 * @brief Estructura empleada como nodo de la lista enlazada empleada en la tabla hash. Almacena un registro del archivo de datos sin procesar.
 */
typedef struct record {
   int sourceId;
   int destId;
   int hourOfDay;
   float meanTravelTime;
   struct record *next;
} record_t;

/**
 * @brief Lista enlazada empleada para almacenar temporalmente los registros del archivo de datos sin procesar, y luego crear el archivo binario a partir de la misma.
 */
typedef struct recordLinkedList {
   record_t * head;
} recordList_t;

/**
 * @brief Estructura empleada para escribir los datos de cada registro en el archivo binario.
 */
typedef struct recordRead {
   int sourceId;
   int destId;
   int hourOfDay;
   float meanTravelTime;
} recordRead_t;

/**
 * @brief Estructura empleada para almacenar los datos de consulta del cliente.
 */
typedef struct recordQuery {
   int sourceId;
   int destId;
   int hourOfDay;
} recordQuery_t;
