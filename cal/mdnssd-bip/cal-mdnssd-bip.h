
//
// common header file for mDNS-SD/BIP CAL Client & Server code
//


#ifndef __CAL_MDNSSD_BIP_H
#define __CAL_MDNSSD_BIP_H


#include <stdint.h>

#include <glib.h>

#include "cal-util.h"




#define BIP_MSG_MAX_SIZE (1024 * 1024)

#define BIP_MSG_HEADER_SIZE (5)

#define BIP_MSG_HEADER_TYPE_OFFSET (0)
#define BIP_MSG_HEADER_SIZE_OFFSET (1)

#define BIP_MSG_TYPE_MESSAGE   (0)
#define BIP_MSG_TYPE_SUBSCRIBE (1)
#define BIP_MSG_TYPE_PUBLISH   (2)




typedef struct {
    char *hostname;  //!< DNS hostname, or IP address as a dotted quad ascii string
    uint16_t port;   //!< in host byte order

    int socket;      //!< the socket connected to this peer, or -1 if we're not currently connected

    //! the header of the packet we're currently receiving
    char header[BIP_MSG_HEADER_SIZE];
    int header_index;

    //! the payload of the packet we're currently receiving
    //! (dynamically allocated)
    int msg_size;  //!< size of message we're receiving (we get this from the header)
    char *buffer;  //!< buffer holding the message payload we're receiving
    int index;     //!< number of bytes of payload received so far
} bip_peer_network_info_t;


typedef struct {
    bip_peer_network_info_t *net;  // NULL if the peer is not currently on the network
    GSList *subscriptions;         // each is a dynamically allocated string of the topic  (FIXME: only the server uses this)
} bip_peer_t;




#define Max(a, b) ((a) > (b) ? (a) : (b))


// FIXME: if the cal users (clients & servers) could somehow pass in the
//     first part of this string, cal could be used by different
//     applications on the same physical network
#define CAL_MDNSSD_BIP_SERVICE_NAME "_bionet._tcp"




// 
// helper functions used by both the Client and Server *threads*.
//


void bip_net_free(bip_peer_network_info_t *net);
void bip_peer_free(bip_peer_t *peer);


//! 
//! \brief Sends a message to a peer
//! 
//! \param peer The peer to send the message to.  Must be already
//!     connected.
//! 
//! \param msg_type The message type to send.
//!
//! \param msg The payload of the message.
//!
//! \param size The number of bytes in msg to send.
//!
//! \return 0 on success, -1 on error.
//!

int bip_send_message(const char *peer_name, const bip_peer_t *peer, uint8_t msg_type, const void *msg, uint32_t size);


//! 
//! \brief Reads data from a peer, up to one complete packet.
//!
//! \param peer_name The name of the peer we're reading from.
//!
//! \param peer The peer to read from.  The data will be read into the end
//!     of the peer's buffer.
//!
//! \return -1 on failure (in which case the peer needs to be disconnected
//!     by the caller).  0 indicates some data was read but a complete
//!     packet is not available yet.  1 indicates exactly one packet is in
//!     the peer's buffer.
//!

int bip_read_from_peer(const char *peer_name, bip_peer_t *peer);


//! 
//! \brief Drops whatever packet (or partial packet) is in the net struct.
//!
//! \param net The network into to clear.
//!
//! \return Nothing.
//!

void bip_net_clear(bip_peer_network_info_t *net);




#endif  // __CAL_MDNSSD_BIP_H
