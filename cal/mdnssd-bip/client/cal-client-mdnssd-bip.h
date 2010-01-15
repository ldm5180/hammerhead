
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
    int (*peer_matches)(const char *peer_name, const char *subscription);
    int running;
    GSList *service_list;
    GList *connecting_peer_list;
    //
    // the key is a peer name
    // the value is a bip_peer_t pointer if the peer is known, NULL if it's unknown
    GHashTable *peers;

    GPtrArray *subscriptions;


} cal_client_mdnssd_bip_t;



extern char *cal_client_mdnssd_bip_network_type;

extern void (*cal_client_mdnssd_bip_callback)(cal_event_t *event);

// pipes between CAL Client thread and user thread
bip_msg_queue_t bip_client_msgq;




// the Client thread
void *cal_client_mdnssd_bip_function(void *arg);


extern void cal_client_mdnssd_bip_thread_destroy(cal_client_mdnssd_bip_t* thread_data);


#endif  //  __CAL_MDNSSD_BIP_CLIENT_H

