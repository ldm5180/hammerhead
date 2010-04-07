
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#ifndef BIP_SOCKET_API_H
#define BIP_SOCKET_API_H

/*
 * This is the socket api used by bip for any plaform
 *
 * There is a different implementation of these functions 
 * for each platform supported
 */

#include "config.h"
#include "cal-mdnssd-bip.h"

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#define SSL_ERROR_ERNO_VAL EILSEQ

#else
// POSIX
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>

#define SSL_ERROR_ERNO_VAL EPROTO

#endif



/**
 * @brief Set the blocking mode of a socket
 *
 * @param s The socket to modify
 * @param blocking Blocking setting. 1 for blocking mode, 0 for non-blocking
 *
 * @retval 0 on success
 * @retval -1 on error. An error message has been logged
 */
int bip_socket_set_blocking(int s, int blocking);

/**
 * @brief Connect to the socket
 *
 * Connect to the socket, logging any errors
 *
 * @retval 0 The socket has started the connect process
 * @retval -1 An error occurred
 */
int bip_socket_connect(
        int s, 
        const struct sockaddr *address,
        socklen_t address_len,
        const char * peer_name,
        const bip_peer_network_info_t * net);

/**
 * @brief Listen on a socket
 *
 * @retval >= 0 The socket that was created
 * @retval -1 An error occurred, and a mesage was logged
 */
int bip_socket_listen(int s, int backlog);

/**
 * @brief Create a tcp socket
 *
 * @retval >= 0 The socket that was created
 * @retval -1 An error occurred, and a mesage was logged
 */
int bip_socket_tcp(void);

#endif // BIP_SOCKET_API_H
