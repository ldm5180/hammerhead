
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef __CAL_SERVER_MDNSSD_BIP_H
#define __CAL_SERVER_MDNSSD_BIP_H


#include <sys/select.h>
#include <dns_sd.h>
#include <glib.h>

#include "cal-util.h"

#include "cal-mdnssd-bip.h"

#include <openssl/bio.h>



#define ID "mDNS-SD/BIP Server "


// describes "this" server
typedef struct {
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

} cal_server_mdnssd_bip_t;


extern char *cal_server_mdnssd_bip_network_type;

extern void (*cal_server_mdnssd_bip_callback)(cal_event_t *event);

void* cal_server_mdnssd_bip_function(void *this_as_voidp);

// Pipes to/from user.
// TODO: Move into cal_Server_mdnssd_bip_t
extern bip_msg_queue_t bip_server_msgq;

void cal_server_mdnssd_bip_shutdown(void);
int cal_server_mdnssd_bip_read(struct timeval *timeout);


void cal_server_mdnssd_bip_destroy(cal_server_mdnssd_bip_t * data);


#endif  //  __CAL_SERVER_MDNSSD_BIP_H

