// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

// This library is free software. You can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as 
// published by the Free Software Foundation, version 2.1 of the License.
// This library is distributed in the hope that it will be useful, but 
// WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details. A copy of the GNU 
// Lesser General Public License v 2.1 can be found in the file named 
// “COPYING.LESSER”.  You should have received a copy of the GNU Lesser 
// General Public License along with this library; if not, write to the 
// Free Software Foundation, Inc., 
// 51 Franklin Street, Fifth Floor, 
// Boston, MA 02110-1301 USA.
 
// You may contact the Automation Group at:
// bionet@bioserve.colorado.edu
 
// Dr. Kevin Gifford
// University of Colorado
// Engineering Center, ECAE 1B08
// Boulder, CO 80309
 
// Because BioNet was developed at a university, we ask that you provide
// attribution to the BioNet authors in any redistribution, modification, 
// work, or article based on this library.
 
// You may contribute modifications or suggestions to the University of
// Colorado for the purpose of discussing and improving this software.
// Before your modifications are incorporated into the master version 
// distributed by the University of Colorado, we must have a contributor
// license agreement on file from each contributor. If you wish to supply
// the University with your modifications, please join our mailing list.
// Instructions can be found on our website at 
// http://bioserve.colorado.edu/developers-corner.

#include <stdlib.h>
#include <sys/types.h>

#ifndef BPS_SOCKET_H
#define BPS_SOCKET_H

/**
 * @file bps_socket.h
 * Bundle Protocol with Sockets interface
 *
 * Wrapper around DTN Bundle Protocol (rfc5050) implementation(s) using a socket-like interface
 */


#ifdef __cplusplus
extern "C"
{
#endif

#define AF_DTN


typedef struct bps_sockaddr
{
	char uri[BPS_EID_SZ];
} bps_sockaddr;
typedef bps_sockaddr *in_bpsaddr;

/**
 * Create a socket for bps communication.
 * 
 * A selectable file descriptor will be returned that can be used 
 * by other bps functions, and select()
 *
 * Compare to socket(2)
 *
 *
 * @param[in] domain 
 *   Ignored. Would be AF_DTN if merged with socket(2)
 * @param[in] type 
 *   Ignored. Would be SOCK_BUNDLE if merged with socket(2)
 * @param[in] protocol 
 *   Ignored. Should be 0 for rfc5050
 *
 * @return
 *   file descriptor for thid DTN socket. The file decscriptor can be used
 *   with select(), poll(), and other bps_*() functions.
 */
extern int bps_socket(int domain, int type, int protocol);


/**
 * Set bps soclket options
 *
 * Like setsockopt(2), but for bps sockets
 *
 * @param[in] sockfd
 *   Socket file descriptor returned by some bps_*() function
 * @param[in] level 
 *   The socket API level to set. Only SOL_SOCKET is supported.
 * @param[in] optname 
 *   Option to set. One of:
 *     - TBD
 * @param[in] optval 
 *   Set option to this value. Type is specified by optname
 * @param[in] optlen 
 *   Size of buffer pointed to by optval
 *
 * @return On success, zero is returned
 * @return On error, -1 is returned, and errno is set appropriately. 
 */
extern int bps_setsockopt(int sock_fd, int level, int optname, 
        void *optval, socklen_t optlen);

/**
 * Set bps soclket options
 *
 * Like getsockopt(2), but for bps sockets
 *
 * @param[in] sockfd
 *   Socket file descriptor returned by some bps_*() function
 * @param[in] level 
 *   The socket API level to set. Only SOL_SOCKET is supported.
 * @param[in] optname 
 *   Option to get. One of:
 *     - TBD
 * @param[out] optval 
 *   Buffer to store option value in. Type is specified by optname
 * @param[in,out] optlen 
 *   Size of buffer pointed to by optval. Set to the number of bytes copied
 *   on return
 *
 * @return On success, zero is returned
 * @return On error, -1 is returned, and errno is set appropriately. 
 */
extern int bps_getsockopt(int sockfd, int level, int optname, 
        void *optval, socklen_t *optlen);


/**
 * Bind to an endpoint identifier.
 *
 * If this is called before sending bundle(s), then those bundles will have
 * this EID listed as the Source EID
 *
 * This is required before receiving bundles with recv*, or listen/accept
 * 
 * @param[in] sockfd
 *   File descriptor returned by bps_socket()
 * @param[in] addr
 *   Address to set as source EID.
 * @param[in] addrlen
 *   Size of buffer pointed to by addr
 *
 * @return On success, zero is returned
 * @return On error, -1 is returned, and errno is set appropriately. 
 */
extern int bps_bind(int sockfd, struct bps_sockaddr *addr, socklen_t addrlen);

/**
 * Connect to the specified endpoint
 *
 * Returns a file descriptor that is "connected" to the remote endpoint. Any
 * bundles sent over this sockfd will got to that EID.
 *
 * @param[in] sockfd
 *   File descriptor returned by bps_socket()
 * @param[in] addr
 *   Address to set as source EID.
 * @param[in] addrlen
 *   Size of buffer pointed to by addr
 *
 * @return On success, a new bps socket file descriptor
 * @return On error, -1 is returned, and errno is set appropriately. 
 */
extern int bps_connect(int dtn_fd, struct bps_sockaddr *addr, socklen_t addrlen);

/**
 * Put the bound socket in listening mode
 *
 * This sets up the socket to receive bundles in a "connectionish" fashion.
 *
 * sockfd will be readable by select() when a bundle is available. The user
 * should then call accept to receive a bundle file descriptor to read the
 * bundle's payload as a stream
 *
 * The socket can still be used by sendto() to send bundles to arbitrary 
 *
 *
 * @param[in] sockfd
 *   A bound bps socket.
 * @param[in] backlog
 *   How many bundles to receive in the background
 */
extern int bps_listen(int sockfd, int backlog);


/**
 * Accept a bundle.
 *
 * This returns a bps socket representing a single bundle.
 *
 * When reading, the new socket will return the single bundle's payload
 *
 * When writing, the new socket behavies like a normal "connected" socket,
 * and will send one or more bundles to the accepted bundle's source EID
 *
 * @param[in] sockfd
 *   File descriptor returned by bps_socket(), that has been put in the 
 *   listening state by bps_listen()
 * @param[out] addr
 *   Buffer to hold the source endpoint ID of the accepted bundle.
 *   May be NULL to ignore source address
 * @param[in,out] addrlen
 *   Size of buffer pointed to by addr. Set to numberof bytes copied on success
 *
 * @return On success, a new bps socket file descriptor
 * @return On error, -1 is returned, and errno is set appropriately. 
 */
extern int bps_accept(int sockfd, struct bps_sockaddr *addr, socklen_t *addrlen);


/**
 * Read bytes from socket
 *
 * see recv(2)
 *
 * For all bps socket types except an accepted socket, bundles larger than
 * len will be truncated to fit the supplyed buffer, and the remainder 
 * discarded.
 *
 * For accepted sockets, subsequent calls will return remaining data.
 *
 * FLAGS:
 *   - MSG_OOB: Receive the bundle header as bps_bdl_headr_t. Must be called
 *              before receiving any data from the bundle
 *   - MSG_TRUNC: Return the rael length of the bundle, even when it
 *                is longer than the passed buffer
 *
 * @param[in] sockfd The bps socket
 * @param[out] buf Buffer to copy bytes into
 * @param[in] len Number of bytes to read.
 * @param[in] flags Bitwise OR of any of the FLAGS
 *
 * @return the number of bytes copied
 */
extern int bps_recv(int sockfd, void *buf, size_t len, int flags);


/**
 * Read bytes from socket
 *
 * see recvfrom(2)
 *
 * For all bps socket types except an accepted socket, bundles larger than
 * len will be truncated to fit the supplyed buffer, and the remainder 
 * discarded.
 *
 * For accepted sockets, subsequent calls will return remaining data.
 *
 * FLAGS:
 *   - MSG_OOB: Receive the bundle header as bps_bdl_headr_t. Must be called
 *              before receiving any data from the bundle
 *   - MSG_TRUNC: Return the rael length of the bundle, even when it
 *                is longer than the passed buffer
 *
 * @param[in] sockfd The bps socket
 * @param[out] buf Buffer to copy bytes into
 * @param[in] len Number of bytes to read.
 * @param[in] flags Bitwise OR of any of the FLAGS
 * @param[out] src_addr The source address of this bundle
 * @param[in,out] addrlen Size of src_addr buffer. Set to number of bytes 
 * copied into src_addr on success
 *
 * @return the number of bytes copied into buf
 * @return -1 on error. errno set accordingly
 */
extern int bps_recvfrom(int sockfd, void *buf, size_t len, int flags, 
        struct bps_sockaddr *src_addr, socklen_t *adderlen);


/**
 * Write bytes to a connected bps socket
 *
 * see send(2)
 *
 * For all socket types, each call to send will generate a bundle, unless
 * the MSG_MORE flag is set.
 *
 * FLAGS:
 *   - MSG_MORE: Don't send the bundle yet. Append this data to the bundle,
 *               and wait for the next call to bps_send()
 *
 * @param[in] sockfd The bps socket
 * @param[in] buf Buffer containing bytes to send
 * @param[in] len Number of bytes in buf
 * @param[in] flags Bitwise OR of any of the FLAGS
 *
 * @return On success, return the number of bytes sent
 * @return On error, -1. errno set accordingly
 */
extern int bps_send(int sockfd, void *buf, size_t len, int flags);

/**
 * Send bundle to an enpoint
 *
 * see sendto(2)
 *
 * For all socket types, each call to send will generate and send a bundle.
 *
 * FLAGS:
 *   - TBD
 *
 * @param[in] sockfd The bps socket
 * @param[in] buf Buffer containing bytes to send
 * @param[in] len Number of bytes in buf
 * @param[in] flags Bitwise OR of any of the FLAGS
 * @param[in] dst_addr The endpoint to send this bundle to
 * @param[in] addrlen size of src_addr buffer. 
 *
 * @return On success, return the number of bytes sent
 * @return On error, -1. errno set accordingly
 */
extern int bps_sendto(int sockfd, void *buf, size_t len, int flags, 
        struct bps_sockaddr *dst_addr, socklen_t adderlen);

/**
 * Close the bps socket and destroys any memory held internally for 
 * this socket.
 *
 * @param sockfd The bps socket to close
 */
extern void bps_close(int sockfd);

#ifdef __cplusplus
}
#endif

#endif // BPS_SOCKET_H

