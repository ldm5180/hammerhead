
///
/// \file cal-peer.h
///
/// \brief the CAL Peer interface
///
/// \author Sebastian Kuzminsky <seb@highlab.com>
///

#ifndef __CAL_PEER_H
#define __CAL_PEER_H


#include <stdint.h>




//! These are the addressing schemes supported by CAL
typedef enum {
    CAL_AS_NONE = 0,   //!< uninitialized addressing scheme
    CAL_AS_IPv4        //!< IPv4
} cal_as_type_t;


//!
//! This structure handles communication with a peer over TCP/IPv4.
//!
//! When used by a client to represent a server (one that's listening for
//! connections), this contains the address that the server is listening on;
//! the socket is the client's connection to the server or -1 if the client
//! has not connected to this peer yet.
//!
//! When used by a server to represent a connected client peer, this
//! contains the address that the peer connected from, and the socket of the
//! connection.
//!
//! When used by a server to represent itself, the address is the server's
//! listening address (hostname may be uninitialized or INADDR_ANY
//! "0.0.0.0"), and the socket is the server's listening socket.
//!

typedef struct {
    int socket;      //!< the socket connected to this peer, or -1 if we're not currently connected
    char *hostname;  //!< DNS hostname, or IP address as a dotted quad ascii string
    uint16_t port;   //!< in host byte order
} cal_as_ipv4_t;


typedef struct {
    //! freeform ascii string (FIXME: no cryptographic protection or certification yet)
    char *name;

    //! Selects which of the "as" union members is active & valid.
    cal_as_type_t addressing_scheme;

    union {
        cal_as_ipv4_t ipv4;
    } as;

    //! it's up to the user to manage this one
    void *user_data;
} cal_peer_t;




//!
//! \brief Allocates a new CAL Peer.
//!
//! \param name The name of the new peer.  Optional, will be ignored if NULL.
//!
//! \return A dynamically allocated CAL Peer (optionally with name set).
//!     peer->addressing_scheme will be initalized to CAL_AS_NONE, the
//!     caller may intialize the new peer's address information by calling
//!     cal_peer_set_addressing_scheme().  The caller is responsible for
//!     freeing the peer later.
//!

cal_peer_t *cal_peer_new(const char *name);




//!
//! \brief Frees a CAL Peer.
//!
//! It's the caller's responsibility to manage the peer->user_data pointer.
//! This function will emit a warning message if peer->user_data is not
//! NULL, but the peer will still be deallocated.
//!
//! \param peer The peer to free.
//!

void cal_peer_free(cal_peer_t *peer);




//!
//! \brief Initializes the addressing scheme of a peer.
//!
//! Any existing addressing information for this peer is lost (the caller
//! should clean up any old addressing scheme before calling this
//! function).  The specified new addressing scheme is initalized and made
//! usable for the caller.
//!
//! For CAL_AS_IPv4 this means: socket=-1, hostname=NULL, port=0
//!
//! \param peer The peer to initialize the addressing scheme for.
//! 
//! \param addressing_scheme The new addressing scheme to initialize for this peer.
//!

void cal_peer_set_addressing_scheme(cal_peer_t *peer, cal_as_type_t addressing_scheme);




//!
//! \brief Makes a human-readable string description of a peer's addressing information.
//!
//! \param peer The peer whose addressing information should be shown.
//!
//! \return A statically allocated string containing a human-readable
//!     representation of the peer's current addressing information.  For
//!     CAL_AS_IPv4 this takes the form "IPv4://hostname:port".
//!     The returned string should not be modified or freed by the caller.
//!     Its contents will remain valid until the next call to this
//!     function, at which point they're overwritten.
//!

const char *cal_peer_address_to_str(const cal_peer_t *peer);




#endif //  __CAL_PEER_H

