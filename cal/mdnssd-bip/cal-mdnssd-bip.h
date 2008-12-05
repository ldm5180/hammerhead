
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
    GPtrArray *nets;          // each pointer is a bip_peer_network_info_t*
    GSList *subscriptions;   // each is a dynamically allocated string of the topic  (FIXME: only the server uses this)
} bip_peer_t;




#define Max(a, b) ((a) > (b) ? (a) : (b))




// 
// helper functions used by both the Client and Server *threads*.
//




/**
 * @brief Allocates and initalizes a bip_peer_network_info_t structure.
 *
 * @param hostname The hostname corresponding to the new net.
 *     The bip_net_new function copies the string, so the called can do
 *     what they want with their copy when the function returns.
 *
 * @param port The port number corresponding to the new net.  In host byte
 *     order.
 *
 * @return On success, returns the new structure, ready to use.
 *
 * @return On failure, returns NULL.
 */

bip_peer_network_info_t *bip_net_new(const char *hostname, uint16_t port);




/**
 * @brief Connects a bip net.
 *
 * @param peer_name The name of the peer whose net to connect to.  Only
 *     used for log messages.
 *
 * @param net The net to connect.
 *
 * @return The connected socket on success.
 *
 * @return -1 on failure (in which case the caller should destroy the net).
 */

int bip_net_connect(const char *peer_name, bip_peer_network_info_t *net);




/**
 * @brief Checks to see if a net is connected or not
 *
 * @param net The net to check.
 *
 * @return FALSE (zero) if the net is not connected.
 *
 * @return TRUE (non-zero) if the net *is* connected.
 */

int bip_net_is_connected(bip_peer_network_info_t *net);




/**
 * @brief Disconnects a bip net.
 *
 * @param net The net to disconnect.
 */

void bip_net_disconnect(bip_peer_network_info_t *net);




/** 
 * @brief Drops whatever packet (or partial packet) is in the net struct.
 *
 * @param net The network info to clear.
 *
 * @return Nothing.
 */

void bip_net_clear(bip_peer_network_info_t *net);




/**
 * @brief Deallocates all the storage of a bip_peer_network_info_t structure.
 *
 * @param net  The structure to free.
 */

void bip_net_free(bip_peer_network_info_t *net);




/**
 * @brief Cleanly destroys a net, no matter what state it's in.
 *
 * If the net is connected, this function calls bip_net_disconnect().
 * Then it calls bip_net_free().
 *
 * @param net  The structure to destroy.
 */

void bip_net_destroy(bip_peer_network_info_t *net);




/**
 * @brief Deallocates a bip_peer_t structure.
 *
 * Closes any open connection to the peer, and destroys all the peer's
 * network info.
 *
 * FIXME: this is really more of a destructor, because it does a bunch of
 * stuff in addition to freeing the allocated memory.
 *
 * @param peer  The peer structure to free.
 */

void bip_peer_free(bip_peer_t *peer);




/**
 * @brief Creates a new bip_peer.
 *
 * @return A pointer to the new, initialized peer on success.
 * @return NULL on failure
 */

bip_peer_t *bip_peer_new(void);




/**
 * @brief Connect to a peer.
 *
 * If the peer already has a connected net, no action is taken.
 *
 * The function tries to connect to each of the peer's nets, removing any
 * that fail to connect.  If the peer runs out of nets, it returns failure.
 *
 * @param peer_name The name of the peer to connect to.  Only used for log
 *     messages.
 *
 * @param peer The peer to connect to.
 *
 * @return 0 if the peer is now connected.
 *
 * @return -1 if no connection was possible, and all the nets have been exhausted.
 */

int bip_peer_connect(const char *peer_name, bip_peer_t *peer);




/**
 * @brief Disconnect from a peer.
 *
 * If the peer is already disconnected, no action is taken.
 *
 * @param peer The peer to disconnect from.
 *
 */

void bip_peer_disconnect(bip_peer_t *peer);




/**
 * @brief Finds the first connected net of a peer.
 *
 * @param peer The peer to examine.
 *
 * @return If a connected net is found, it is returned.  If no connected
 *     net is found, NULL is returned.
 */

bip_peer_network_info_t *bip_peer_get_connected_net(const bip_peer_t *peer);




/**
 * @brief Sends a message to a peer.
 *
 * The peer must already be connected.
 *
 * @param peer_name The name of the peer (used only for log messages).
 *
 * @param peer The peer to send the message to.
 *
 * @param msg_type The message type to send.
 *
 * @param msg The payload of the message.
 *
 * @param size The number of bytes in msg to send.
 *
 * @return 0 on success.
 *
 * @return -1 on error, in which case the peer's connected net should be closed.
 */

int bip_send_message(const char *peer_name, const bip_peer_t *peer, uint8_t msg_type, const void *msg, uint32_t size);




/** 
 * @brief Reads data from a peer, up to one complete packet.
 *
 * @param peer_name The name of the peer we're reading from.
 *
 * @param peer The peer to read from.  The first connected net will be
 *     used.  The data will be read into the end of the peer's buffer.
 *
 * @return A return value of 1 indicates that exactly one packet is in the
 *     net's buffer.
 *
 * @return A return value of 0 indicates some data was read but a complete
 *     packet is not available yet.
 *
 * @return A return value of -1 indicates failure, and the caller should
 *     disconnect the peer's connected net.
 */

int bip_read_from_peer(const char *peer_name, bip_peer_t *peer);




#endif  // __CAL_MDNSSD_BIP_H
