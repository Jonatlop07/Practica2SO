/**
 * @file clientQueue.h
 * @version 1.0
 * @date 02/06/2021
 * @author Jonathan Lopez Castellanos - Víctor Alfredo Barragán Paez
 * @title Cola para almacenar la información de los clientes
 * @brief Contiene la definición de la estructura nodo y las funciones de la cola: enqueue y dequeue.
 */
#ifndef CLIENTQUEUE_H_
#define CLIENTQUEUE_H_

#include <stdlib.h>
#include <stdio.h>
#include "../types/client.h"

/**
 * @brief Estructura que contiene los datos de conexion del cliente (fd y datos del socket). Empleada en la cola de clientes.
 */
typedef struct node {
   client_t *data;
   struct node *next;
} node_t;

/**
 * @brief enqueue Crea una variable de tipo nodo_t, le inserta los datos del cliente y la enlaza al final de la cola.
 * @param client Apuntador con los datos del cliente.
 */
void enqueue( client_t* );

/**
 * @brief dequeue Remueve la variable de tipo nodo_t de la cabeza y retorna los datos del cliente si esta existe. De lo contrario, retorna NULL.
 * @return Apuntador con los datos del cliente
 */
client_t* dequeue();

#endif
