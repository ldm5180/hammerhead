
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

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

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
    char * peer_name;
};

static GSList *service_list = NULL;

static GList *connecting_peer_list = NULL;



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

    peer->peer_name = strdup(peer_name);
    if (peer->peer_name == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "get_peer_by_name: out of memory");
        bip_peer_free(peer);
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




static void report_peer_lost(bip_peer_t *peer) {
    cal_event_t *event;
    int r;

    event = cal_event_new(CAL_EVENT_LEAVE);
    if (event == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "report_peer_lost: out of memory!");
        return;
    }

    event->peer_name = strdup(peer->peer_name);
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


// NOTE: This assumes control of 'msg_data', which will be free'd
// later as a result of calling this function
static void _peer_add_pending_msg(
    bip_peer_t *peer,
    uint8_t msg_type,
    void *msg_data,
    uint32_t size)
{
    bip_msg_t * msg = malloc(sizeof(bip_msg_t));
    if(NULL == msg) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_send_message: Out of memory");
        free(msg);
        return;
    }

    msg->type = msg_type;
    msg->data = msg_data;
    msg->size = size;

    peer->pending_msgs = g_slist_append(peer->pending_msgs, msg);

}


/**
 * @brief Sends all matching queued-up subscriptions to this connected peer.
 *
 * This assumed that the peer's first net is connected and writable. If not, this 
 * call may block
 *
 * @param peer The peer to subscribe from.
 *
 * @return 0 on success.
 *
 * @return -1 on failure, in which case the connected net's connection is broken. 
 *     caller must handle this error by reporting a LOST_PEER, before attempting to connect to
 *     annother available net
 */

int send_subscriptions(bip_peer_t *peer) {
    int r = 0;
    int i;

    if (peer == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "send_subscriptions: NULL peer passed in");
        return -1;
    }

    if (peer->nets->len == 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "send_subscriptions: passed-in peer has no nets");
        return -1;
    }

    for (i = 0; i < subscriptions->len; i ++) {
        cal_client_mdnssd_bip_subscription_t *s;

        s = (cal_client_mdnssd_bip_subscription_t *)g_ptr_array_index(subscriptions, i);
        if (s == NULL) continue;

        if (this->peer_matches(peer->peer_name, s->peer_name) != 0) continue;

        r = bip_send_message(peer, BIP_MSG_TYPE_SUBSCRIBE, s->topic, strlen(s->topic) + 1);
        if (r != 0) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                ID "send_subscriptions: error sending subscription to peer '%s'", 
                    peer->peer_name);
            break;
        }
    }

    return r;
}

static void reset_connection(bip_peer_t * peer) {

    report_peer_lost(peer);

    bip_peer_disconnect(peer);
    int fd = bip_peer_connect_nonblock(peer);
    if(fd >= 0) {
        // The next net is not ready yet
        connecting_peer_list = g_list_append(connecting_peer_list, peer);
    }
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

            r = bip_send_message(peer, BIP_MSG_TYPE_MESSAGE, 
                event->msg.buffer, event->msg.size);
            if (r != 0) {
                // Save this message to be re-sent on re-connect
                _peer_add_pending_msg(peer, BIP_MSG_TYPE_MESSAGE, 
                    event->msg.buffer, event->msg.size);
                event->msg.buffer = NULL; // Free'd when the peer finally sends the msg
                event->msg.size = 0;
                reset_connection(peer);
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
                    if (bip_peer_get_connected_net(peer) == NULL) continue;
                    if (this->peer_matches(name, s->peer_name) != 0) continue;

                    r = bip_send_message(peer, BIP_MSG_TYPE_SUBSCRIBE, 
                        s->topic, strlen(s->topic) + 1);
                    if (r != 0) {
                        reset_connection(peer);
                    }
                }
            }

            break;
        }

        case CAL_EVENT_UNSUBSCRIBE: {
            cal_client_mdnssd_bip_subscription_t *s;
            int i;
            int matching_subscription_exists = 0;

            if (!cal_peer_name_is_valid(event->peer_name)) {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read_from_user: user Unsubscribe event has invalid peer_name, ignoring");
                break;
            }

            if (!cal_topic_is_valid(event->topic)) {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read_from_user: user Unsubscribe event has invalid topic, ignoring");
                break;
            }

            //
            // finding & removing all (one) matching subscription
            //

            i = 0;
            while (i < subscriptions->len) {
                s = (cal_client_mdnssd_bip_subscription_t*)g_ptr_array_index(subscriptions, i);

                if ((strcmp(event->peer_name, s->peer_name) == 0) &&
                    (strcmp(event->topic, s->topic) == 0)) {

                    s = (cal_client_mdnssd_bip_subscription_t*)g_ptr_array_remove_index(subscriptions, i);

                    if (s->peer_name != NULL) free(s->peer_name);
                    if (s->topic != NULL) free(s->topic);
                    free(s);

                    matching_subscription_exists = 1;

                    break;
                } else {
                    i++;
                }
            }

            //
            // walk the list of known peers, and send this unsubscribe message to all that match
            //

            if (matching_subscription_exists) {
                GHashTableIter iter;
                const char *name;
                bip_peer_t *peer;

                g_hash_table_iter_init (&iter, peers);
                while (g_hash_table_iter_next(&iter, (gpointer)&name, (gpointer)&peer)) {
                    if (bip_peer_get_connected_net(peer) == NULL) continue;
                    if (this->peer_matches(name, event->peer_name) != 0) continue;

                    r = bip_send_message(peer, BIP_MSG_TYPE_UNSUBSCRIBE, 
                        event->topic, strlen(event->topic) + 1);
                    if (r != 0 ) {
                        reset_connection(peer);
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
        reset_connection(peer);
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



//
// Called when mDNS resolves an address for a host name.
// May be called multiple times per published service (multi-home)
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

    struct cal_client_mdnssd_bip_service_context *sc = context;
    char * peer_name = sc->peer_name;

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
        free(peer_name);
        return;
    }

    peer = get_peer_by_name(peer_name);
    free(peer_name);
    if (peer == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "resolve_callback: out of memory");
        return;
    }

    net = bip_net_new(hosttarget, ntohs(port));
    if (net == NULL) {
        return;
    }

    const void * value;
    uint8_t valueLen;
    value = TXTRecordGetValuePtr(txtLen, txtRecord, "txtvers", &valueLen);
    if( value
    &&  valueLen == sizeof(bip_txtvers_t) 
    &&  BIP_TXTVERS >= *((bip_txtvers_t*)value))
    {
	value = TXTRecordGetValuePtr(txtLen, txtRecord, "sec", &valueLen);
	if( value ) {
	    if ( 0 == strncmp(value, "req", valueLen) ) {
		net->sectype = BIP_SEC_REQ;	
	    } else if ( 0 == strncmp(value, "opt", valueLen) ) {
		net->sectype = BIP_SEC_OPT;
	    }
	}
    }


    g_ptr_array_add(peer->nets, net);


    if (peer->nets->len > 1) {
        // this peer was known, initalized, and reported to the user before
        // no need for any more action this time
        return;
    }


    //
    // New peer just showed up on the network. Push it on the list of hosts to connect to
    // 
    if(bip_peer_connect_nonblock(peer) < 0) {
        return;
    }
    connecting_peer_list = g_list_append(connecting_peer_list, peer);

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

        sc = malloc(sizeof(struct cal_client_mdnssd_bip_service_context));
        if (sc == NULL) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "browse_callback: out of memory!");
            return;
        }

        sc->peer_name = strdup(name);

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
            free(sc->peer_name);
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
        free(sc->peer_name);
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


static void free_peer(void *peer_as_void) {
    bip_peer_t *peer = peer_as_void;

    if (peer == NULL) return;

    // this cleans up all the peer's nets too
    bip_peer_free(peer);
}

// Call after connect has succeeded to:
//   1 - Send any subscriptions matching this peer
//   2 - Send all pending messages for this peer
//   3 - Send PEER_JOIN to user
//
// If there is an error sending, attempt to re-connect
// 
static void post_connect(bip_peer_t * peer) {
    int r;
    cal_event_t * event;

    // Send subscriptions first. No point notifying the user of a new peer if
    // we can't communicate with it
    r = send_subscriptions(peer);
    if (r != 0) {
        reset_connection(peer);
        return;
    }

    // Send any pending messages before notifying user thread
    GSList *ptr;
    while ((ptr = peer->pending_msgs)) {
        bip_msg_t * msg = ptr->data;
        r = bip_send_message(peer, msg->type, msg->data, msg->size);
        if (r != 0) {
            reset_connection(peer);
            return;
        }

        free(msg->data);
        free(msg);
        ptr->data = NULL;
        peer->pending_msgs = g_slist_delete_link(peer->pending_msgs, ptr);
    }

    // Notify user thread of new peer
    event = cal_event_new(CAL_EVENT_JOIN);
    if (event == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "new_connection: out of memory!");
        return;
    }

    event->peer_name = strdup(peer->peer_name);
    if (event->peer_name == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "new_connection: out of memory!");
        cal_event_free(event);
        return;
    }

    event->is_secure = bip_peer_is_secure(peer);

    // send the Join event
    // the event becomes the responsibility of the callback now, so they might leak memory but we're not
    r = write(cal_client_mdnssd_bip_fds_to_user[1], &event, sizeof(event));  // heh
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "new_connection: error writing event: %s", strerror(errno));
        cal_event_free(event);
    } else if (r != sizeof(event)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "new_connection: short write while writing event");
        cal_event_free(event);
    }

    // 'event' passes out of scope here, but we don't leak its memory
    // because we have successfully sent a pointer to it to the user thread
    // coverity[leaked_storage]
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

    // Initialize SSL library
    //SSL_load_error_strings();
    ERR_load_BIO_strings();
    // TODO: Lots more

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
        fd_set writers;
        int max_fd;
        int r;
        GSList *ptr;
        GList *dptr;

SELECT_LOOP_CONTINUE:

        FD_ZERO(&readers);
        FD_ZERO(&writers);
        max_fd = -1;


        // each DNS-SD service request has its own fd
        for (ptr = service_list; ptr != NULL; ptr = ptr->next) {
            struct cal_client_mdnssd_bip_service_context *sc = ptr->data;
            int fd = DNSServiceRefSockFD(sc->service_ref);
            FD_SET(fd, &readers);
            max_fd = Max(max_fd, fd);
        }

        // each connect-pending socket
        for ( dptr = connecting_peer_list; dptr != NULL; dptr = dptr->next) {
            bip_peer_t *peer = dptr->data;
            if ( peer->nets->len > 0 ) {
                bip_peer_network_info_t *net;
                net = g_ptr_array_index(peer->nets, 0);
                int fd = net->socket;
                if(net->pending_bio && BIO_should_read(net->pending_bio)){
                    FD_SET(fd, &readers);
                } else {
                    FD_SET(fd, &writers);
                    FD_SET(fd, &readers);
                }
                max_fd = Max(max_fd, fd);
            }
        }

        // the user thread might want to say something
        FD_SET(cal_client_mdnssd_bip_fds_from_user[0], &readers);
        max_fd = Max(max_fd, cal_client_mdnssd_bip_fds_from_user[0]);

        // each server we're connected to might want to say something,
        // or have data to be sent
        {
            GHashTableIter iter;
            const char *name;
            bip_peer_t *peer;

            g_hash_table_iter_init (&iter, peers);
            while (g_hash_table_iter_next(&iter, (gpointer)&name, (gpointer)&peer)) {
                bip_peer_network_info_t *net = bip_peer_get_connected_net(peer);
                if (net == NULL) continue;
                FD_SET(net->socket, &readers);
                if(net->write_pending) {
                    FD_SET(net->socket, &writers);
                }
                max_fd = Max(max_fd, net->socket);
            }
        }

        // block until there's something to do
        r = select(max_fd + 1, &readers, &writers, NULL, NULL);

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

        // see if any of our servers said/read anything
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
                    goto SELECT_LOOP_CONTINUE;
                }

                if (FD_ISSET(net->socket, &writers)) {
                    r = bip_drain_pending_msgs(net);
                    if ( r < 0 ) {
                        // peer should be discnnected
                        reset_connection(peer);
                        goto SELECT_LOOP_CONTINUE;
                    }
                }
            }
        }

        // See if any connect()s have finished. 
        for ( dptr = connecting_peer_list; dptr != NULL; dptr = dptr->next) {
            bip_peer_t *peer = dptr->data;
            if ( peer->nets->len > 0 ) {
                bip_peer_network_info_t *net;
                int fd;

                net = g_ptr_array_index(peer->nets, 0);
                fd = net->socket;
                if (FD_ISSET(fd, &writers) || FD_ISSET(fd, &readers)) {
                    r = bip_peer_connect_finish(peer);
                    if (r == 0 ) continue; // Call again later

                    connecting_peer_list = g_list_delete_link(connecting_peer_list, dptr);
                    if( r < 0 ) {
                        // This connect failed. Try the next one
                        int fd = bip_peer_connect_nonblock(peer);
                        if(fd >= 0) {
                            // The next net is not ready yet
                            connecting_peer_list = g_list_append(connecting_peer_list, peer);
                        }
                    } else {
                        // Do post-connect stuff
                        post_connect(peer);
                    }
                    goto SELECT_LOOP_CONTINUE;
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

