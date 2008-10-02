
// 
// This header file describes the Bionet 2 Communication Abstraction Layer.
//

#ifndef __CAL_H
#define __CAL_H


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




typedef enum {
    CAL_EVENT_NONE = 0,
    CAL_EVENT_JOIN,       // we've noticed that a peer has joined the network
    CAL_EVENT_LEAVE,      // we've noticed that a peer has left the network
    CAL_EVENT_CONNECT     // a peer has connected to us
} cal_event_type_t;


typedef struct {
    cal_event_type_t type;
    cal_peer_t *peer;
} cal_event_t;


cal_event_t *cal_event_new(cal_event_type_t type);
void cal_event_free(cal_event_t *event);




typedef struct {
    //
    // these make a peer "findable" by others:
    //

    // Inform the network that the peer "me" has joined
    // returns TRUE (non-zero) on success, FALSE (zero) on failure
    int (*join)(cal_peer_t *me);

    // Inform the network that the peer "me" has left
    // returns TRUE (non-zero) on success, FALSE (zero) on failure
    int (*leave)(cal_peer_t *me);


    // 
    // these let a peer see what other peers are out there:
    //

    // starts tracking the peer list
    // creates a file descriptor for the user to monitor, the user must call cal_pd->read() when the fd is readable
    // returns the fd, or -1 on error
    int (*subscribe_peer_list)(void (*callback)(cal_event_t *event));

    // user should call this function when the fd returned from subscribe() is readable
    // this function may call the callback registered with the subscribe() call
    // returns TRUE (non-zero) on success, FALSE (0) on failure
    int (*read)(void);

    // stops tracking the peer list
    // the fd from subscribe_peer_list() is invalid after this function returns
    void (*unsubscribe_peer_list)(void);
} cal_pd_t;




typedef struct {
    //
    // publisher stuff
    //

    // fills out this->addr and this->socket
    int (*init_publisher)(cal_peer_t *this, void (*callback)(cal_event_t *event));
    void (*publish)(char *topic, void *msg, int size);
    int (*publisher_read)(void);  // maybe calls callback


    //
    // subscriber stuff
    //

    int (*init_subscriber)(void (*callback)(cal_event_t *event));
    void (*cancel_subscriber)(void);
    void (*subscribe)(cal_peer_t *peer, char *topic);
    int (*subscriber_read)(void);  // maybe calls callback


    //
    // usable by both
    //

    int (*sendto)(cal_peer_t *peer, void *msg, int size);

} cal_i_t;




#endif //  __CAL_H

