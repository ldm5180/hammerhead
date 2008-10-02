
#ifndef __CAL_PEER_H
#define __CAL_PEER_H


#include <stdint.h>

#include <sys/socket.h>




typedef enum {
    CAL_AS_NONE = 0,
    CAL_AS_IPv4
} cal_as_type_t;


//
// This structure handles communication with a peer over TCP/IPv4.
//
// When used by a client to represent a server (one that's listening for
// connections), this contains the address that the server is listening on;
// the socket is the client's connection to the server or -1 if the client
// has not connected to this peer yet.
//
// When used by a server to represent a connected client peer, this
// contains the address that the peer connected from, and the socket of the
// connection.
//
// When used by a server to represent itself, the address is the server's
// listening address (hostname may be uninitialized or INADDR_ANY
// "0.0.0.0"), and the socket is the server's listening socket.
//

typedef struct {
    int socket;
    char *hostname;
    uint16_t port;   // in host byte order
} cal_as_ipv4_t;


typedef struct {
    char *name;  // FIXME: freeform ascii string, no cryptographic protection or certification yet

    cal_as_type_t addressing_scheme;
    union {
        cal_as_ipv4_t ipv4;
    } as;

    // it's up to the user to manage this one
    void *user_data;
} cal_peer_t;


cal_peer_t *cal_peer_new(const char *name);
void cal_peer_free(cal_peer_t *peer);
void cal_peer_set_addressing_scheme(cal_peer_t *peer, cal_as_type_t addressing_scheme);
const char *cal_peer_address_to_str(const cal_peer_t *peer);




#endif //  __CAL_PEER_H

