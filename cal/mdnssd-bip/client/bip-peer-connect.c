
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bip-socket-api.h"

#include <glib.h>

#include "bip-mdnssd-api.h"

#include "cal-mdnssd-bip.h"
#include "cal-client-mdnssd-bip.h"



#ifdef HAVE_EMBEDDED_MDNSSD
static void DNSSD_API get_addr_info_callback(
    DNSServiceRef service_ref,
    DNSServiceFlags flags,
    uint32_t interfaceIndex,
    DNSServiceErrorType errorCode,
    const char *hosttarget,
    const struct sockaddr *address,
    uint32_t ttl,
    void *context
) {
    struct cal_client_mdnssd_bip_service_context *sc = context;
    char * peer_name = sc->peer_name;
    cal_client_mdnssd_bip_t * this = sc->this;

    bip_peer_t *peer;
    bip_peer_network_info_t *me = NULL;
    int i;

    // no mutex necessary because it should already be locked
    DNSServiceRefDeallocate(service_ref);
    this->service_list = g_slist_remove(this->service_list, sc);
    free(sc);

    // which peer do we correspond to?
    peer = g_hash_table_lookup(this->peers, peer_name);
    free(peer_name);
    if (peer == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "addr_info_callback: peer does not exist!");
        return;
    }

    // which net wanted us to get their sock addr info?
    for (i = 0; i < peer->nets->len; i++) {
        bip_peer_network_info_t *net;
        net = g_ptr_array_index(peer->nets, i);

        // sanity-check: if a socket has already been created we should already
        // have the struct sockaddr, so return
        if (net->socket >= 0)
            return;

        if (strcmp(hosttarget, net->hostname) == 0)
            me = net;
    }

    if (me == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "addr_info_callback: bip_peer_network_info_t does not exist!");
        // this should only happen if the peer was deleted before
        // this callback was called, so don't try to connect again
        return;
    }

    if (errorCode != kDNSServiceErr_NoError) {
        if (errorCode != kDNSServiceErr_Unknown) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "addr_info_callback: Error returned from GetAddrInfo: %d", errorCode);
        }
    } else { 
        // no errors occured & we have the address info, so create a non-blocking socket
        if (bip_net_connect_nonblock((void*)this, peer->peer_name, me, address) >= 0) {
            this->connecting_peer_list = g_list_append(this->connecting_peer_list, peer);
            return;
        }
    }

    // 
    // here, we either failed to create a non-blocking socket on this net, 
    // or failed to properly get the sockaddr info. 
    // 

    // in either case, remove this net...
    g_ptr_array_remove_fast(peer->nets, me);
    bip_net_destroy(me);

    // and try to connect to the next one
    start_bip_peer_connect_nonblock(this, peer);

    return;
}
#endif

 

static int start_get_addr_info(cal_client_mdnssd_bip_t *this, bip_peer_t *peer, bip_peer_network_info_t *net) {
#ifdef HAVE_EMBEDDED_MDNSSD
    struct cal_client_mdnssd_bip_service_context *sc;
    DNSServiceErrorType error;

    sc = malloc(sizeof(struct cal_client_mdnssd_bip_service_context));
    if (sc == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "bip_peer_connect: out of memory!");
        return -1;
    }

    sc->peer_name = strdup(peer->peer_name);
    sc->this = this;

    error = DNSServiceGetAddrInfo(
        &sc->service_ref,
        0,
        0,
        kDNSServiceType_A,
        net->hostname,
        get_addr_info_callback,
        sc
    );
    if (error != kDNSServiceErr_NoError) {
        if (error != kDNSServiceErr_Unknown) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "bip_peer_connect: GetAddrInfo failed: %d", error);
        }

        free(sc->peer_name);
        free(sc);
    } else {
        // this link will be removed by the get_addr callback
        sc->this->service_list = g_slist_prepend(sc->this->service_list, sc);
        return 1;
    }
#else
    struct addrinfo ai_hints, *ai;
    int r;

    // must gethostaddrinfo here ... 
    memset(&ai_hints, 0, sizeof(ai_hints));

    ai_hints.ai_family = AF_INET;  // IPv4
    ai_hints.ai_socktype = SOCK_STREAM;  // TCP
    r = getaddrinfo(net->hostname, NULL, &ai_hints, &ai);
    if (r != 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "%s: error with getaddrinfo(\"%s\", ...): %s",
                __FUNCTION__, net->hostname, gai_strerror(r));
        return -1;
    }
    if (ai == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "%s: no results from getaddrinfo(\"%s\", ...)",
                __FUNCTION__, net->hostname);
        return -1;
    }


    int fd = bip_net_connect_nonblock((void *)this, peer->peer_name, net, ai->ai_addr);
    if ( fd >= 0 ) {
        this->connecting_peer_list = g_list_append(this->connecting_peer_list, peer);
        return fd;
    }

    freeaddrinfo(ai);

#endif
    //g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_peer_connect: unable to connect to any net");
    return -1;
}




// Returns fd of connection if its in-progress, -1 if there are no more nets
int start_bip_peer_connect_nonblock(void * cal_handle, bip_peer_t * peer) {
    cal_client_mdnssd_bip_t * this = (cal_client_mdnssd_bip_t *)cal_handle;
    int r;

    if (cal_handle == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_peer_connect: NULL cal handle passed in");
        return -1;
    }

    if (peer == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_peer_connect: NULL peer passed in");
        return -1;
    }


    // keep trying to a net until there are none left
    while (peer->nets->len > 0) {
        bip_peer_network_info_t *net;
        net = g_ptr_array_index(peer->nets, 0);

        if (net == NULL)
            break;

        if ((r = start_get_addr_info(this, peer, net)) > 0) {
            return r;
        }

        g_ptr_array_remove_fast(peer->nets, net);
        bip_net_destroy(net);
    }

    //g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_peer_connect: unable to connect to any net");
    return -1;
}


//
// Finish the connect procedure. Call this when the peer->net
// connect socket is writable:
// 
// If there was an error, then the failed net will be removed from the peer. Its up to the caller to try the next one
//
// @return bio, if connected. NULL on error
int bip_peer_connect_finish(void * cal_handle, bip_peer_t * peer) {
    cal_client_mdnssd_bip_t * this = (cal_client_mdnssd_bip_t *)cal_handle;
    int r = -1;

    if (cal_handle == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_peer_connect: NULL cal handle passed in");
        return -1;
    }

    if (peer == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_peer_connect: NULL peer passed in");
        return -1;
    }
    

    if ( peer->nets->len > 0 ) {
        bip_peer_network_info_t *net = g_ptr_array_index(peer->nets, 0);
        r = bip_net_connect_check((void *)this, peer->peer_name, net);
        if(r < 0) {
            g_ptr_array_remove_fast(peer->nets, net);
            bip_net_destroy(net);
        }
    }

    return r;
}
