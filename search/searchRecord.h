/**
 * @file searchRecord.h
 * @version 1.0
 * @date 02/06/2021
 * @author Jonathan López Castellanos - Víctor Alfredo Barragán Paez
 * @title Función de búsqueda de registros
 * @brief Realiza la búsqueda del tiempo medio de viajes del archivo proporcionado por Uber.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "../utils/errorHandler.h"
#include "../types/record.h"

#define TRUE 1
#define FALSE 0

/**
 * @brief searchRecordMeanTravelTime Función que recibe los datos de consulta, hace la búsqueda de la posición del primer registro con el id de origen ingresado en una tabla hash almacenada en disco, para luego comparar los registros a partir de dicha posición hasta encontrar el registro buscado y retornar el tiempo medio de viajes o retornar cero en caso de no encontrar el registro.
 * @param queryParameters Datos de la consulta (id de origen, id de destino y hora del día).
 * @return Tiempo medio de viajes.
 */
float searchRecordMeanTravelTime( recordQuery_t * );

