
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


//
// common header file for mDNS-SD/BIP CAL Client & Server code
//


#ifndef __CAL_MDNSSD_BIP_H
#define __CAL_MDNSSD_BIP_H


#include <stdint.h>

#include <glib.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/x509v3.h>
#include <openssl/bio.h>

#include "cal-util.h"
#include "shared/bip-itc.h"

#define BIP_MAX_WRITE_BUF_SIZE (1024 * 256)

#define BIP_MSG_MAX_SIZE (1024 * 1024)

#define BIP_MSG_HEADER_SIZE (5)

#define BIP_MSG_HEADER_TYPE_OFFSET (0)
#define BIP_MSG_HEADER_SIZE_OFFSET (1)

#define BIP_MSG_TYPE_MESSAGE       (0)
#define BIP_MSG_TYPE_SUBSCRIBE     (1)
#define BIP_MSG_TYPE_PUBLISH       (2)
#define BIP_MSG_TYPE_UNSUBSCRIBE   (3)


/* security configuration files */
#define BIP_PUBLIC_CERT     "pub.cert"
#define BIP_PRIVATE_KEY     "prv.key"
#define BIP_CA_DIR          "ca-dir"
#define BIP_SECURITY_SERIAL "serial"
#define BIP_SSL_CONF        "bionet-ssl.conf"
#define BIP_CA_FILE         NULL
#define BIP_CA_CERTFILE     "ca-cert.pem"

/* BIP mDNS-SD Txt record values */
#define BIP_TXTVERS 1
typedef uint8_t bip_txtvers_t;

typedef enum {
    BIP_SEC_NOT_CONNECTED = -1,
    BIP_SEC_NONE          = 0,
    BIP_SEC_REQ           = 1,
    BIP_SEC_OPT           = 2
} bip_sec_type_t;

typedef struct {
    char *hostname;  //!< DNS hostname, or IP address as a dotted quad ascii string
    uint16_t port;   //!< in host byte order

    int socket;      //!< the socket connected to this peer, or -1 if we're not currently connected
    BIO * socket_bio; //!< the BIO that wraps the socket connected to this peer, or NULL if not connected
    BIO * pending_bio; //!< the BIO of the pending BIO connect operation, or NULL if no op pending
    bip_sec_type_t sectype; //!< the type of security adversited by this peer
    bip_sec_type_t security_status;

    //! the header of the packet we're currently receiving
    char header[BIP_MSG_HEADER_SIZE];
    int header_index;

    //! the payload of the packet we're currently receiving
    //! (dynamically allocated)
    int msg_size;  //!< size of message we're receiving (we get this from the header)
    char *buffer;  //!< buffer holding the message payload we're receiving
    int index;     //!< number of bytes of payload received so far

    // Send queue. Msgs queued to be sent to this net. They are invalidated if the net gets disconnected
    uint32_t bytes_queued;  // Number of bytes in the queues below. Used to limit resource consumption
    GQueue msg_send_queue;    // list of message buffers (bip_buf_t) that are pending to be sent when 
                               // the socket becomes writable
    size_t curr_msg_bytes_sent; // Number of bytes sent from the top-most msg
    int write_pending; // If set, this net has data to be sent once the socket becomes writable
    SSL_CTX * ssl_ctx_server;
    SSL_CTX * ssl_ctx_client;
} bip_peer_network_info_t;

typedef struct {
    void * data;
    size_t size;
} bip_buf_t;

typedef struct {
    uint8_t type;
    void * data;
    uint32_t size;
} bip_msg_t;

typedef struct {
    char * peer_name;        // Name of this peer.
    GPtrArray *nets;         // each pointer is a bip_peer_network_info_t*
    GSList *subscriptions;   // each is a dynamically allocated string of the topic  (FIXME: only the server uses this)
    GSList *pending_msgs;    // list of messages (bip_msg_t) that are pending to be sent to peer when connection is established
} bip_peer_t;


typedef struct {
    size_t max_write_buf_size;
} bip_shared_config_t;

extern bip_shared_config_t bip_shared_cfg;

// Initialize the shared config to the default values
// unless overridden by the environment
void bip_shared_config_init(void);


#define Max(a, b) ((a) > (b) ? (a) : (b))

#define cal_pthread_mutex_lock(mutex) \
while (1) \
{ \
    int                status; \
    struct timespec    delay; \
    struct timespec    remtime; \
\
    status = pthread_mutex_lock(mutex); \
        if (0 != status) \
    { \
        if ((status == EINVAL) || (status == EDEADLK)) \
        { \
                        fprintf(stderr, "Invalid or deadlocked mutex!\n"); \
            break; \
        } \
\
        delay.tv_sec = 0; \
        delay.tv_nsec = 100000; \
        remtime.tv_sec = 0; \
        remtime.tv_nsec = 0; \
        (void) nanosleep (&delay, &remtime); \
        continue; \
    } \
    break; \
}

#define cal_pthread_mutex_unlock(mutex) \
while (1) \
{ \
    int                status; \
    struct timespec    delay; \
    struct timespec    remtime; \
\
    status = pthread_mutex_unlock(mutex); \
        if (0 != status) \
    { \
        if ((status == EINVAL) || (status == EDEADLK)) \
        { \
                        fprintf(stderr, "Invalid or deadlocked mutex!\n"); \
            break; \
        } \
\
        delay.tv_sec = 0; \
        delay.tv_nsec = 100000; \
        remtime.tv_sec = 0; \
        remtime.tv_nsec = 0; \
        (void) nanosleep (&delay, &remtime); \
\
        continue; \
    } \
    break; \
}



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
 * @brief Start to connect a bip net.
 *
 * @param peer_name The name of the peer whose net to connect to.  Only
 *     used for log messages.
 *
 * @param net The net to connect.
 *
 * @return The socket that the connection is in progress 
 *
 * @return -1 on failure (in which case the caller should destroy the net).
 */

int bip_net_connect_nonblock(void * cal_handle, const char *peer_name, bip_peer_network_info_t *net);

/**
 * @brief Finish the connect, and check the status
 *
 * @param peer_name The name of the peer whose net to connect to.  Only
 *     used for log messages.
 *
 * @param net The net to check.
 *
 * @return 1 when socket is connected
 *
 * @return -1 on failure (in which case the caller should destroy the net). (errno set)

 * @return 0 if connect is still in progress. Call this function later
 */

int bip_net_connect_check(void * cal_handle, const char *peer_name, bip_peer_network_info_t *net);



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
 * @brief Start connecting to a peer.
 *
 * The function attempts to connect to each of the peer's nets, in a non-blocking
 * fashion. If the peer runs out of nets, it will return failure.
 *
 * To check if the connect succeeded, call bip_peer_connect_finish() after the
 * returned fd is writable (see select())
 *
 * @param peer The peer to connect to.
 *
 * @return fd of the socket that is connection.
 *
 * @return -1 if no connection was possible, and all the nets have been exhausted.
 */

int bip_peer_connect_nonblock(void * cal_handle, bip_peer_t * peer);

/**
 * @brief Connect to a peer.
 *
 * If the peer already has a connected net, no action is taken.
 *
 * The function tries to finish the connection in-progress. Call this function
 * after the socket returned by bip_peer_connect_nonblock() is writable.
 * 
 * On failure, the failed net will be removed from the peer, and the caller should call
 * bip_peer_connect_nonblock() to try the next one.
 *
 * @param peer The peer to connect to.
 *
 * @return -1 if no connection could be established
 *
 * @return 0 if the connection is still pending

 * @return 1 if the connection has been established
 */
int bip_peer_connect_finish(void * cal_handle, bip_peer_t * peer);

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
 * @brief Check if the peer is using a secure connection
 *
 * @param[in] peer The peer to examine
 *
 * @retval 0 Insecure or invalid peer or no connected net.
 * @retval 1 Secure
 */
int bip_peer_is_secure(bip_peer_t *peer);


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

int bip_send_message(const bip_peer_t *peer, uint8_t msg_type, const void *msg, uint32_t size);




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



/**
 * @brief Verifies an X509 certificate
 *
 * @param[in] ok TRUE if the certificate is ok so far
 * @param[in] store The store of current certificates
 *
 * @return 0 Failure
 * @return Non-zero Success
 */
int bip_ssl_verify_callback(int ok, X509_STORE_CTX *store);

/**
 * @brief Send pending data in the peer's write queue
 *
 * @param peer
 *   The peer whose socket is writable
 *
 * @return 0 on success
 * @return -1 There is an error sending data, and the peer should be reset by caller
 */
int bip_drain_pending_msgs(bip_peer_network_info_t *net);

#endif  // __CAL_MDNSSD_BIP_H
