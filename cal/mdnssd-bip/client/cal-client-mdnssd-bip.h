
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef __CAL_MDNSSD_BIP_CLIENT_H
#define __CAL_MDNSSD_BIP_CLIENT_H


#include <dns_sd.h>
#include <glib.h>
#include <pthread.h>

#include "cal-mdnssd-bip.h"
#include "../shared/bip-socket-api.h"




#define ID "mDNS-SD/BIP Client "


#ifdef HAVE_EMBEDDED_MDNSSD
extern mDNS mDNSStorage;
#endif


extern pthread_mutex_t avahi_mutex;

typedef struct {
    //!
    //! \brief A callback function provided by the user, to be called by
    //!     the CAL Client library whenever an event requires the user's
    //!     attention.
    //!
    //! Set by .init(), called by .read()
    //!
    //! The events are documented in the cal_event_t enum, in the
    //! cal-event.h file.
    //!
    //! \param event The event that requires the user's attention.  The
    //!     event is const, so the callback function should treat it as
    //!     read-only.
    //!

    void (*callback)(void * cal_handle, const cal_event_t *event);

    int (*peer_matches)(const char *peer_name, const char *subscription);
    int running;
    GSList *service_list;
    GList *connecting_peer_list;
    //
    // the key is a peer name
    // the value is a bip_peer_t pointer if the peer is known, NULL if it's unknown
    GHashTable *peers;

    GPtrArray *subscriptions;

    char *cal_client_mdnssd_bip_network_type;

    void (*cal_client_mdnssd_bip_callback)(cal_event_t *event);

    // pipes between CAL Client thread and user thread
    bip_msg_queue_t msg_queue;


    GThread *client_thread;

    SSL_CTX * ssl_ctx_client;
    bip_sec_type_t client_require_security;
} cal_client_mdnssd_bip_t;


// the Client thread
void *cal_client_mdnssd_bip_function(void *arg);


extern void cal_client_mdnssd_bip_thread_destroy(cal_client_mdnssd_bip_t* thread_data);

/**
 * @brief Start to connect a bip net.
 *
 * @param peer_name The name of the peer whose net to connect to.  Only
 *     used for log messages.
 *
 * @param net The net to connect.
 *
 * @param saddr The sock addr info struct
 *
 * @return The socket that the connection is in progress 
 *
 * @return -1 on failure (in which case the caller should destroy the net).
 */

int bip_net_connect_nonblock(void * cal_handle, const char *peer_name, bip_peer_network_info_t *net, const struct sockaddr *saddr);


//
// this is a linked list, each payload is a (struct cal_client_mdnssd_bip_service_context *)
// we add the first one when we start the mDNS-SD browse running, then we add one each time we start a resolve
// when the resolve finishes we remove its node from the list
//

struct cal_client_mdnssd_bip_service_context {
    DNSServiceRef service_ref;
    char * peer_name;
    cal_client_mdnssd_bip_t * this;
};

typedef struct {
    char *peer_name;
    char *topic;
} cal_client_mdnssd_bip_subscription_t;


#endif  //  __CAL_MDNSSD_BIP_CLIENT_H

