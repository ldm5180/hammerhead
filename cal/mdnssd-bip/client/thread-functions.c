
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <glib.h>
#include <dns_sd.h>

#include "cal-client.h"
#include "cal-mdnssd-bip.h"
#include "cal-client-mdnssd-bip.h"




static cal_client_mdnssd_bip_t *this = NULL;




// 
// this is a linked list, each payload is a (struct cal_client_mdnssd_bip_service_context *)
// we add the first one when we start the mDNS-SD browse running, then we add one each time we start a resolve
// when the resolve finishes we remove its node from the list
//

struct cal_client_mdnssd_bip_service_context {
    DNSServiceRef service_ref;
    cal_event_t *event;
};

static GSList *service_list = NULL;




// the key is a peer name
// the value is a bip_peer_t pointer if the peer is known, NULL if it's unknown
static GHashTable *peers = NULL;




typedef struct {
    char *peer_name;
    char *topic;
} cal_client_mdnssd_bip_subscription_t;

static GPtrArray *subscriptions = NULL;




/**
 * @brief Looks up a server peer, by name.  Creates the peer if it doesnt
 *     already exist.
 *
 * @param peer_name The name of the peer to find.
 *
 * @return The bip_peer_t, if found-or-created.
 *
 * @return NULL on out-of-memory.
 */

bip_peer_t *get_peer_by_name(const char *peer_name) {
    char *hash_key;
    bip_peer_t *peer;

    peer = g_hash_table_lookup(peers, peer_name);
    if (peer != NULL) return peer;

    peer = bip_peer_new();
    if (peer == NULL) {
        return NULL;
    };

    hash_key = strdup(peer_name);
    if (hash_key == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "get_peer_by_name: out of memory");
        bip_peer_free(peer);
        return NULL;
    };

    g_hash_table_insert(peers, hash_key, peer);

    return peer;
}




void report_peer_lost(const char *peer_name, bip_peer_t *peer) {
    cal_event_t *event;
    int r;

    event = cal_event_new(CAL_EVENT_LEAVE);
    if (event == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "report_peer_lost: out of memory!");
        return;
    }

    event->peer_name = strdup(peer_name);
    if (event->peer_name == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "report_peer_lost: out of memory!");
        cal_event_free(event);
        return;
    }

    // the event and the peer become the responsibility of the user's callback now, so they might leak memory but we're not
    r = write(cal_client_mdnssd_bip_fds_to_user[1], &event, sizeof(event));  // heh
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "browser_callback: error writing event: %s", strerror(errno));
        cal_event_free(event);
        return;
    } else if (r != sizeof(event)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "browser_callback: short write while writing event");
        cal_event_free(event);
        return;
    }

    // 'event' passes out of scope here, but we don't leak its memory
    // because we have successfully sent a pointer to it to the user thread
    // coverity[leaked_storage]
    return;
}




void net_leaves(bip_peer_t *peer, bip_peer_network_info_t *net) {
    if (peer == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "net_leaves: NULL peer passed in");
        return;
    }

    if (net == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "net_leaves: NULL net passed in");
        return;
    }

    g_ptr_array_remove(peer->nets, net);
    bip_net_destroy(net);
}




void peer_leaves(bip_peer_t *peer) {
    if (peer == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "peer_leaves: NULL peer passed in");
        return;
    }

    while (peer->nets->len > 0) {
        bip_peer_network_info_t *net;
        net = g_ptr_array_index(peer->nets, 0);
        net_leaves(peer, net);
    }
}




/**
 * @brief Sends all matching queued-up subscriptions to this peer.
 *
 * If the peer's not not connected yet, it tries to connect first.  Any
 * nets that fail to connect are removed from the peer's net list.
 *
 * @param peer_name The name of the peer we're sending subscriptions to.
 *     Only used for log messages.
 *
 * @param peer The peer to subscribe from.
 *
 * @return 0 on success.
 *
 * @return -1 on failure, in which case the peer is all out of nets and
 *     probably needs to be reported lost by the caller.
 */

int send_subscriptions(const char *peer_name, bip_peer_t *peer) {
    if (peer == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "send_subscriptions: NULL peer passed in");
        return -1;
    }

    if (peer->nets->len == 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "send_subscriptions: passed-in peer has no nets");
        return -1;
    }

    while (peer->nets->len > 0) {
        int r;
        int i;

        r = bip_peer_connect(peer_name, peer);
        if (r < 0) {
            // g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "send_subscriptions: error connecting to peer '%s'", peer_name);
            return -1;
        }

        for (i = 0; i < subscriptions->len; i ++) {
            cal_client_mdnssd_bip_subscription_t *s;
            int r;

            s = (cal_client_mdnssd_bip_subscription_t *)g_ptr_array_index(subscriptions, i);
            if (s == NULL) continue;

            if (this->peer_matches(peer_name, s->peer_name) != 0) continue;

            r = bip_send_message(peer_name, peer, BIP_MSG_TYPE_SUBSCRIBE, s->topic, strlen(s->topic) + 1);
            if (r != 0) {
                // g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "send_subscriptions: error sending subscription to peer '%s'", peer_name);
                bip_peer_disconnect(peer);
                break;
            }
        }

        // if we sent all the subscriptions without disconnecting, we're done here
        if (bip_peer_get_connected_net(peer) != NULL) return 0;
    }

    return -1;
}




// this function is called by the thread main function when the user thread wants to tell it something
static void read_from_user(void) {
    cal_event_t *event;
    int r;

    r = read(cal_client_mdnssd_bip_fds_from_user[0], &event, sizeof(event));
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read_from_user: error reading from user: %s", strerror(errno));
        return;
    } else if (r != sizeof(event)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read_from_user: short read from user");
        return;
    }

    if (event == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read_from_user: ignoring NULL event from user!");
        return;
    }

    switch (event->type) {
        case CAL_EVENT_MESSAGE: {
            bip_peer_t *peer;

            if (!cal_peer_name_is_valid(event->peer_name)) {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read_from_user: user Message event has invalid peer_name, ignoring");
                break;
            }

            if (event->msg.buffer == NULL) {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read_from_user: user Message event has NULL msg buffer, ignoring");
                break;
            }

            if (event->msg.size < 1) {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read_from_user: user Message event has invalid size %d, ignoring", event->msg.size);
                break;
            }

            peer = get_peer_by_name(event->peer_name);
            if (peer == NULL) break;
            if (peer->nets->len == 0) break;

            while (peer->nets->len > 0) {
                r = bip_peer_connect(event->peer_name, peer);
                if (r < 0) {
                    report_peer_lost(event->peer_name, peer);
                    break;
                }

                r = bip_send_message(event->peer_name, peer, BIP_MSG_TYPE_MESSAGE, event->msg.buffer, event->msg.size);
                if (r == 0) break;
            }

            break;
        }


        case CAL_EVENT_SUBSCRIBE: {
            cal_client_mdnssd_bip_subscription_t *s;


            if (!cal_peer_name_is_valid(event->peer_name)) {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read_from_user: user Subscribe event has invalid peer_name, ignoring");
                break;
            }

            if (!cal_topic_is_valid(event->topic)) {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read_from_user: user Subscribe event has invalid topic, ignoring");
                break;
            }


            //
            // first record the new subscription in the master subscription list
            //

            s = (cal_client_mdnssd_bip_subscription_t *)calloc(1, sizeof(cal_client_mdnssd_bip_subscription_t));
            if (s == NULL) {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "read_from_user: out of memory");
                return;
            }

            s->peer_name = event->peer_name;
            event->peer_name = NULL;

            s->topic = event->topic;
            event->topic = NULL;

            g_ptr_array_add(subscriptions, s);


            //
            // next walk the list of known peers, and send this subscription to any that match
            //

            {
                GHashTableIter iter;
                const char *name;
                bip_peer_t *peer;

                g_hash_table_iter_init (&iter, peers);
                while (g_hash_table_iter_next(&iter, (gpointer)&name, (gpointer)&peer)) {
                    if (peer->nets->len == 0) continue;
                    if (this->peer_matches(name, s->peer_name) != 0) continue;

                    while (peer->nets->len > 0) {
                        r = bip_peer_connect(name, peer);
                        if (r < 0) {
                            report_peer_lost(name, peer);
                            break;
                        }

                        r = bip_send_message(name, peer, BIP_MSG_TYPE_SUBSCRIBE, s->topic, strlen(s->topic) + 1);
                        if (r == 0) break;
                    }
                }
            }

            break;
        }

        default: {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read_from_user: unknown event %d from user", event->type);
            return;
        }
    }

    cal_event_free(event);
}




// this function is called by the thread main function when a connected publisher has something to say
static void read_from_publisher(const char *peer_name, bip_peer_t *peer, bip_peer_network_info_t *net) {
    cal_event_t *event;
    int r;

    r = bip_read_from_peer(peer_name, peer);
    if (r < 0) {
        bip_net_disconnect(net);

        r = send_subscriptions(peer_name, peer);
        if (r < 0) report_peer_lost(peer_name, peer);

        return;
    }

    // message is not here yet, wait for more to come in
    if (r == 0) return;


    // 
    // exactly one message is in the peer's buffer, handle it now
    //


    switch (net->header[BIP_MSG_HEADER_TYPE_OFFSET]) {
        case BIP_MSG_TYPE_MESSAGE: {
            event = cal_event_new(CAL_EVENT_MESSAGE);
            if (event == NULL) {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "read_from_publisher: out of memory!");
                bip_net_clear(net);
                return;
            }

            event->peer_name = strdup(peer_name);
            if (event->peer_name == NULL) {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "read_from_publisher: out of memory!");
                cal_event_free(event);
                bip_net_clear(net);
                return;
            }

            // the event steals the BIP message buffer 
            event->msg.size = net->msg_size;
            event->msg.buffer = net->buffer;
            net->buffer = NULL;

            break;
        }

        case BIP_MSG_TYPE_PUBLISH: {
            event = cal_event_new(CAL_EVENT_PUBLISH);
            if (event == NULL) {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "read_from_publisher: out of memory!");
                bip_net_clear(net);
                return;
            }

            event->peer_name = strdup(peer_name);
            if (event->peer_name == NULL) {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "read_from_publisher: out of memory!");
                cal_event_free(event);
                bip_net_clear(net);
                return;
            }

            // the event steals the BIP message buffer 
            event->msg.size = net->msg_size;
            event->msg.buffer = net->buffer;
            net->buffer = NULL;

            break;
        }

        default: {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read_from_publisher: invalid BIP message type %d!", net->header[BIP_MSG_HEADER_TYPE_OFFSET]);
            bip_net_clear(net);
            return;
        }
    }

    bip_net_clear(net);

    r = write(cal_client_mdnssd_bip_fds_to_user[1], &event, sizeof(event));
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read_from_publisher: error writing to user thread!!");
        cal_event_free(event);
        return;
    } else if (r < sizeof(event)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read_from_publisher: short write to user thread!!");
        cal_event_free(event);
        return;
    }

    // 'event' passes out of scope here, but we don't leak its memory
    // because we have successfully sent a pointer to it to the user thread
    // coverity[leaked_storage]
    return;
}




static void resolve_callback(
    DNSServiceRef service_ref,
    DNSServiceFlags flags,
    uint32_t interfaceIndex,
    DNSServiceErrorType errorCode,
    const char *fullname,
    const char *hosttarget,
    uint16_t port,
    uint16_t txtLen,
    const unsigned char *txtRecord,
    void *context
) {

    int r;
    struct cal_client_mdnssd_bip_service_context *sc = context;
    cal_event_t *event = sc->event;

    bip_peer_t *peer;
    bip_peer_network_info_t *net;


    // remove this service_ref from the list
    DNSServiceRefDeallocate(sc->service_ref);
    service_list = g_slist_remove(service_list, sc);
    free(sc);

    if (errorCode != kDNSServiceErr_NoError) {
        if (errorCode != kDNSServiceErr_Unknown) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "resolve_callback: Error returned from resolve: %d", errorCode);
        }
        goto fail0;
    }

    peer = get_peer_by_name(event->peer_name);
    if (peer == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "resolve_callback: out of memory");
        goto fail0;
    }

    net = bip_net_new(hosttarget, ntohs(port));
    if (net == NULL) {
        goto fail0;
    }

    g_ptr_array_add(peer->nets, net);


    if (peer->nets->len > 1) {
        // this peer was known, initalized, and reported to the user before
        // no need for any more action this time
        cal_event_free(event);
        return;
    }


    // 
    // New peer just showed up on the network, send it all our subscriptions & report it to the user
    //


    r = send_subscriptions(event->peer_name, peer);
    if (r != 0) {
        // the peer's net list is now empty
        goto fail0;
    }


    // send the Join event
    // the event becomes the responsibility of the callback now, so they might leak memory but we're not
    r = write(cal_client_mdnssd_bip_fds_to_user[1], &event, sizeof(event));  // heh
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "resolve_callback: error writing event: %s", strerror(errno));
    } else if (r != sizeof(event)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "resolve_callback: short write while writing event");
    }

    return;

fail0:
    cal_event_free(event);
    return;
}




// 
// this function gets called whenever a service of our type, for example "_bionet._tcp", comes or goes
//

static void browse_callback(
    DNSServiceRef service,
    DNSServiceFlags flags,
    uint32_t interfaceIndex,
    DNSServiceErrorType errorCode,
    const char *name,
    const char *type,
    const char *domain,
    void *context
) {
    if (errorCode != kDNSServiceErr_NoError) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "browse_callback: Error returned from browse: %d", errorCode);
        return;
    }


    if (flags & kDNSServiceFlagsAdd) {
        struct cal_client_mdnssd_bip_service_context *sc;
        DNSServiceErrorType error;
        cal_event_t *event;

        event = cal_event_new(CAL_EVENT_JOIN);
        if (event == NULL) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "browse_callback: out of memory!");
            return;
        }

        event->peer_name = strdup(name);
        if (event->peer_name == NULL) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "browse_callback: out of memory!");
            cal_event_free(event);
            return;
        }


        sc = malloc(sizeof(struct cal_client_mdnssd_bip_service_context));
        if (sc == NULL) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "browse_callback: out of memory!");
            cal_event_free(event);
            return;
        }

        sc->event = event;

        // Now create a resolve call to fill out the network info part of the bip_peer
        error = DNSServiceResolve(
            &sc->service_ref,
            0,
            interfaceIndex, 
            name,
            type,
            domain, 
            resolve_callback,
            (void*)sc
        );
        if (error != kDNSServiceErr_NoError) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "browse_callback: failed to start resolv service, dropping this joining peer");
            cal_event_free(event);
            free(sc);
            return;
        }

        // add this new service ref to the list
        service_list = g_slist_prepend(service_list, sc);
    } else {
#if 0
        bip_peer_t *peer;

        peer = g_hash_table_lookup(peers, name);
        if (peer == NULL) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "browse_callback: got an mDNS Leave event for unknown server '%s', ignoring", name);
            return;
        }

        // FIXME: just this peer-net leaves, but which is it?
        // peer_leaves(peer);

        // report_peer_lost(name, peer);
#endif
    }
}




// cancel all pending mDNS-SD service requests
void cleanup_service_list(void *unused) {
    GSList *ptr;

    ptr = service_list;
    while (ptr != NULL) {
        GSList *next = ptr->next;
        struct cal_client_mdnssd_bip_service_context *sc = ptr->data;

        DNSServiceRefDeallocate(sc->service_ref);
        service_list = g_slist_remove(service_list, sc);
        free(sc);
        ptr = next;
    }
}


// clean up the peers hash table
void cleanup_peers(void *unused) {
    g_hash_table_unref(peers);
}


// clean up the subscriptions list
void cleanup_subscriptions(void *unused) {
    int i;

    for (i = 0; i < subscriptions->len; i ++) {
        cal_client_mdnssd_bip_subscription_t *s = g_ptr_array_index(subscriptions, i);
        if (s == NULL) continue;
        if (s->peer_name != NULL) free(s->peer_name);
        if (s->topic != NULL) free(s->topic);
        free(s);
    }

    g_ptr_array_free(subscriptions, 1);
}


void free_peer(void *peer_as_void) {
    bip_peer_t *peer = peer_as_void;

    if (peer == NULL) return;

    // this cleans up all the peer's nets too
    bip_peer_free(peer);
}


//
// this function gets run in the thread started by init()
// canceled by shutdown()
//

void *cal_client_mdnssd_bip_function(void *arg) {
    struct cal_client_mdnssd_bip_service_context *browse;
    DNSServiceErrorType error;

    char mdnssd_service_name[100];
    int r;

    // block all signals in this thread
    {
        sigset_t ss;
        sigfillset(&ss);
        pthread_sigmask(SIG_BLOCK, &ss, NULL);
    }

    this = (cal_client_mdnssd_bip_t *)arg;

    r = snprintf(mdnssd_service_name, sizeof(mdnssd_service_name), "_%s._tcp", cal_client_mdnssd_bip_network_type);
    if (r >= sizeof(mdnssd_service_name)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "client thread: network type '%s' too long!", cal_client_mdnssd_bip_network_type);
        return NULL;
    }

    subscriptions = g_ptr_array_new();
    pthread_cleanup_push(cleanup_subscriptions, NULL);

    // Shutup annoying nag message on Linux.
    setenv("AVAHI_COMPAT_NOWARN", "1", 1);

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    browse = malloc(sizeof(struct cal_client_mdnssd_bip_service_context));
    if (browse == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "client thread: out of memory!");
        return NULL;
    }

    error = DNSServiceBrowse(
        &browse->service_ref,
        0,
        0,
	mdnssd_service_name,
        NULL,  
        browse_callback,
        NULL
    );

    if (error != kDNSServiceErr_NoError) {
        free(browse);
	g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "client thread: Error browsing for service: %d", error);
        if (error == kDNSServiceErr_Unknown) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "client thread: make sure the avahi-daemon is running");
        }
	return NULL;
    }

    service_list = g_slist_prepend(service_list, browse);
    pthread_cleanup_push(cleanup_service_list, NULL);

    peers = g_hash_table_new_full(g_str_hash, g_str_equal, free, free_peer);
    pthread_cleanup_push(cleanup_peers, NULL);


    while (1) {
        fd_set readers;
        int max_fd;
        int r;
        GSList *ptr;

SELECT_LOOP_CONTINUE:

        FD_ZERO(&readers);
        max_fd = -1;


        // each DNS-SD service request has its own fd
        for (ptr = service_list; ptr != NULL; ptr = ptr->next) {
            struct cal_client_mdnssd_bip_service_context *sc = ptr->data;
            int fd = DNSServiceRefSockFD(sc->service_ref);
            FD_SET(fd, &readers);
            max_fd = Max(max_fd, fd);
        }

        // the user thread might want to say something
        FD_SET(cal_client_mdnssd_bip_fds_from_user[0], &readers);
        max_fd = Max(max_fd, cal_client_mdnssd_bip_fds_from_user[0]);

        // each server we're connected to might want to say something
        {
            GHashTableIter iter;
            const char *name;
            bip_peer_t *peer;

            g_hash_table_iter_init (&iter, peers);
            while (g_hash_table_iter_next(&iter, (gpointer)&name, (gpointer)&peer)) {
                bip_peer_network_info_t *net = bip_peer_get_connected_net(peer);
                if (net == NULL) continue;
                FD_SET(net->socket, &readers);
                max_fd = Max(max_fd, net->socket);
            }
        }


        // block until there's something to do
        r = select(max_fd + 1, &readers, NULL, NULL, NULL);


        // see if any DNS-SD service requests finished
        for (ptr = service_list; ptr != NULL; ptr = ptr->next) {
            struct cal_client_mdnssd_bip_service_context *sc = ptr->data;
            int fd = DNSServiceRefSockFD(sc->service_ref);

            if (FD_ISSET(fd, &readers)) {
                DNSServiceErrorType err;

                // this will call the service_ref's callback, which will
                // change the service_list (and possibly known_peers and
                // connected_publishers)
                err = DNSServiceProcessResult(sc->service_ref);
                if (err != kDNSServiceErr_NoError) {
                    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "client thread: Error processing service reference result: %d.", err);
                    sleep(1);
                }

                // since the service_list has changed, we should stop iterating over it
		// Also, the open file descriptors may have changed, so we need to 
		// check select() again
		goto SELECT_LOOP_CONTINUE;
            }
        }

        // see if the user thread said anything
        if (FD_ISSET(cal_client_mdnssd_bip_fds_from_user[0], &readers)) {
            read_from_user();
        }

        // see if any of our servers said anything
        {
            GHashTableIter iter;
            const char *name;
            bip_peer_t *peer;

            g_hash_table_iter_init(&iter, peers);
            while (g_hash_table_iter_next (&iter, (gpointer)&name, (gpointer)&peer)) {
                bip_peer_network_info_t *net;

                net = bip_peer_get_connected_net(peer);
                if (net == NULL) continue;

                if (FD_ISSET(net->socket, &readers)) {
                    read_from_publisher(name, peer, net);
                    // read_from_publisher can disconnect the peer, so we need to stop iterating over it now
                    break;
                }
            }
        }
    }

    //
    // NOT REACHED!
    //

    pthread_cleanup_pop(0);  // don't execute cleanup_peers
    pthread_cleanup_pop(0);  // don't execute cleanup_service_list
    pthread_cleanup_pop(0);  // don't execute cleanup_subscriptions
}

