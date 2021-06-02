/**
 * @file client.h
 * @version 1.0.0
 * @date 02/06/2021
 * @author Jonathan López Castellanos - Víctor Alfredo Barragán Paez
 * @title Estructura client
 * @brief Estructura que almacena los datos del cliente.
 */
#ifndef CLIENT_H_
#define CLIENT_H_

#include <netinet/in.h>

/**
 * @brief Estructura que almacena el fd del socket del cliente y los datos del mismo.
 */
typedef struct client {
   int fd;
   struct sockaddr_in socketInfo;
} client_t;

#endif
