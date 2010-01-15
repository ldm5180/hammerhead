
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef __CAL_MDNSSD_BIP_CLIENT_H
#define __CAL_MDNSSD_BIP_CLIENT_H


#include <dns_sd.h>
#include <glib.h>

#include "cal-mdnssd-bip.h"




#define ID "mDNS-SD/BIP Client "




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


#endif  //  __CAL_MDNSSD_BIP_CLIENT_H

