/**
 * @file errorHandler.h
 * @version 1.0
 * @date 02/06/2021
 * @author Jonathan López Castellanos - Víctor Alfredo Barragán Paez
 * @title Función handleError
 * @brief Función de utilidad empleada para imprimir errores en la salida stderr error estándar.
 */
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief handleError Función auxiliar que recibe el código retornado por el llamado a una función del sistema operativo e imprime la cadena suministrada como argumento en la salida stderr, además de finalizar la ejecución de la rutina donde se llame.
 * @param codigo Código de valor entero, el cual si es menor que 0 indica que ha ocurrido un error en el llamado a una función del sistema operativo.
 * @param mensaje Cadena que se imprime en la salida de error estándar de recibirse un código de error.
 */
void handleError( int, char* );
