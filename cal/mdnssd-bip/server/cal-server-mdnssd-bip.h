
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef __CAL_SERVER_MDNSSD_BIP_H
#define __CAL_SERVER_MDNSSD_BIP_H


#include <dns_sd.h>
#include <glib.h>

#include "cal-util.h"

#include "cal-mdnssd-bip.h"

#include <openssl/bio.h>



#define ID "mDNS-SD/BIP Server "


// describes "this" server
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

    char *name;
    uint16_t port;
    int socket;
    BIO * socket_bio;
    int (*topic_matches)(const char *a, const char *b);

    // Thread State
    GHashTable *clients;

    DNSServiceRef *advertisedRef;
    TXTRecordRef txt_ref;

    GList *accept_pending_list; // List of bip_peer_t

    int running;

    char *cal_server_mdnssd_bip_network_type;

    SSL_CTX * ssl_ctx_server;
    int server_require_security;

    GThread* server_thread;

    bip_msg_queue_t bip_server_msgq;
} cal_server_mdnssd_bip_t;


void* cal_server_mdnssd_bip_function(void *this_as_voidp);


void cal_server_mdnssd_bip_shutdown(void * cal_handle);
int cal_server_mdnssd_bip_read(void * cal_handle, struct timeval *timeout);


void cal_server_mdnssd_bip_destroy(cal_server_mdnssd_bip_t * data);


#endif  //  __CAL_SERVER_MDNSSD_BIP_H

