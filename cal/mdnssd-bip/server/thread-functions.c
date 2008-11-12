
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <netinet/in.h>

#include <sys/socket.h>

#include <glib.h>
#include <dns_sd.h>

#include "cal-util.h"
#include "cal-server.h"
#include "cal-mdnssd-bip.h"
#include "cal-server-mdnssd-bip.h"




// each value is a bip_peer_t*
static GHashTable *clients = NULL;

static DNSServiceRef *advertisedRef = NULL;

cal_server_mdnssd_bip_t *this = NULL;




static void register_callback(
    DNSServiceRef sdRef, 
    DNSServiceFlags flags, 
    DNSServiceErrorType errorCode, 
    const char *name, 
    const char *type, 
    const char *domain, 
    void *context
) {
    if (errorCode != kDNSServiceErr_NoError) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "register_callback: Register callback returned %d", errorCode);
    }
}




static void read_from_user(void) {
    cal_event_t *event;
    int r;

    r = read(cal_server_mdnssd_bip_fds_from_user[0], &event, sizeof(event));
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read_from_user: error reading from user: %s", strerror(errno));
        return;
    } else if (r != sizeof(event)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read_from_user: short read from user");
        return;
    }

    switch (event->type) {

        case CAL_EVENT_MESSAGE: {
            bip_peer_t *peer;

            peer = g_hash_table_lookup(clients, event->peer_name);
            if (peer == NULL) {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read_from_user: unknown peer name '%s' passed in, dropping outgoing Message event", event->peer_name);
                return;
            }
            bip_send_message(event->peer_name, peer, BIP_MSG_TYPE_MESSAGE, event->msg.buffer, event->msg.size);
            // FIXME: bip_send_message might not have worked, we need to report to the user or retry or something

            break;
        }

        case CAL_EVENT_PUBLISH: {
            // each client that's connected to us might be subscribed to this topic
            GHashTableIter iter;
            const char *name;
            bip_peer_t *client;

            g_hash_table_iter_init(&iter, clients);
            while (g_hash_table_iter_next(&iter, (gpointer)&name, (gpointer)&client)) {
                GSList *si;

                if (client->net == NULL) continue;
                if (client->net->socket == -1) continue;

                for (si = client->subscriptions; si != NULL; si = si->next) {
                    const char *sub_topic = si->data;

                    if (this->topic_matches(event->topic, sub_topic) == 0) {
                        bip_send_message(name, client, BIP_MSG_TYPE_PUBLISH, event->msg.buffer, event->msg.size);
                        break;
                    }
                }
            }
            break;
        }

        default: {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read_from_user(): unknown event %d from user", event->type);
            return;  // dont free events we dont understand
        }
    }

    cal_event_free(event);
}




static void accept_connection(cal_server_mdnssd_bip_t *this) {
    int r;
    struct sockaddr_in sin;
    socklen_t sin_len;

    cal_event_t *event;
    bip_peer_t *client;


    client = calloc(1, sizeof(bip_peer_t));
    if (client == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "accept_connection: out of memory");
        return;
    }

    client->net = calloc(1, sizeof(bip_peer_network_info_t));
    if (client->net == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "accept_connection: out of memory");
        goto fail0;
    }

    sin_len = sizeof(struct sockaddr_in);
    client->net->socket = accept(this->socket, (struct sockaddr *)&sin, &sin_len);
    if (client->net->socket < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "accept_connection: error accepting a connection: %s", strerror(errno));
        goto fail1;
    }

    event = cal_event_new(CAL_EVENT_CONNECT);
    if (event == NULL) {
        // an error has been logged already
        goto fail2;
    }


    // use the client's network address as it's "name"
    {
        char name[256];
        int r;

        r = snprintf(name, sizeof(name), "bip://%s:%hu", inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));
        if (r >= sizeof(name)) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "accept_connection: peer name too long");
            goto fail3;
        }

        event->peer_name = strdup(name);
        if (event->peer_name == NULL) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "accept_connection: out of memory");
            goto fail3;
        }
    }


    {
        char *name_key;

        name_key = strdup(event->peer_name);
        if (name_key == NULL) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "accept_connection: out of memory");
            goto fail3;
        }
        g_hash_table_insert(clients, name_key, client);
    }


    r = write(cal_server_mdnssd_bip_fds_to_user[1], &event, sizeof(cal_event_t*));
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "accept_connection(): error writing Connect event: %s", strerror(errno));
    } else if (r != sizeof(cal_event_t*)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "accept_connection(): short write of Connect event!");
    }

    return;


fail3:
    cal_event_free(event);

fail2:
    close(client->net->socket);

fail1:
    free(client->net);

fail0:
    free(client);
}




static void send_disconnect_event(const char *peer_name) {
    int r;
    cal_event_t *event;

    event = cal_event_new(CAL_EVENT_DISCONNECT);
    if (event == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "send_disconnect_event: out of memory");
        return;
    }

    event->peer_name = strdup(peer_name);
    if (event->peer_name == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "send_disconnect_event: out of memory");
        cal_event_free(event);
        return;
    }

    r = write(cal_server_mdnssd_bip_fds_to_user[1], &event, sizeof(cal_event_t*));
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "send_disconnect_event: error writing Disconnect event: %s", strerror(errno));
        cal_event_free(event);
    } else if (r != sizeof(cal_event_t*)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "send_disconnect_event: short write of Disconnect event!");
        cal_event_free(event);
    }
}




// reads from the peer, sends a Message event to the user thread
// returns 0 on success, -1 on failure (in which case the peer has been disconnected and removed from the clients hash)
static int read_from_client(const char *peer_name, bip_peer_t *peer) {
    int r;
    cal_event_t *event;

    r = bip_read_from_peer(peer_name, peer);
    if (r < 0) {
        send_disconnect_event(peer_name);
        g_hash_table_remove(clients, peer_name);  // close the network connection, free all allocated memory for key & value
        return -1;
    }

    // message is not here yet, wait for more to come in
    if (r == 0) return 0;


    // 
    // exactly one message is in the peer's buffer, handle it now
    //


    // the actual event type will be set below
    event = cal_event_new(CAL_EVENT_NONE);
    if (event == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "read_from_client: out of memory!");
        bip_net_clear(peer->net);
        return -1;
    }

    switch (peer->net->header[BIP_MSG_HEADER_TYPE_OFFSET]) {
        case BIP_MSG_TYPE_MESSAGE: {
            event->type = CAL_EVENT_MESSAGE;

            // the event steals the BIP message buffer 
            event->msg.size = peer->net->msg_size;
            event->msg.buffer = peer->net->buffer;
            peer->net->buffer = NULL;

            break;
        }

        case BIP_MSG_TYPE_SUBSCRIBE: {
            char *topic;

            event->type = CAL_EVENT_SUBSCRIBE;

            // FIXME: verify that the topic is a NULL-terminated ASCII string

            // the event steals the BIP message buffer as the topic
            event->topic = peer->net->buffer;
            peer->net->buffer = NULL;

            topic = strdup(event->topic);
            if (topic == NULL) {
                cal_event_free(event);
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "read_from_client: out of memory!");
                bip_net_clear(peer->net);
                return -1;
            }
            peer->subscriptions = g_slist_prepend(peer->subscriptions, topic);

            break;
        }

        default: {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read_from_client: dont know what to do with message type %d", peer->net->header[BIP_MSG_HEADER_TYPE_OFFSET]);
            cal_event_free(event);
            bip_net_clear(peer->net);
            return -1;
        }
    }

    bip_net_clear(peer->net);

    event->peer_name = strdup(peer_name);
    if (event->peer_name == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "read_from_client: out of memory");
        cal_event_free(event);
        return -1;
    }

    r = write(cal_server_mdnssd_bip_fds_to_user[1], &event, sizeof(event));
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read_from_client: error writing to user thread!!");
        return -1;
    } else if (r < sizeof(event)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read_from_client: short write to user thread!!");
        return -1;
    }

    return 0;
}




void cleanup_advertisedRef(void *unused) {
    if (advertisedRef != NULL) {
        DNSServiceRefDeallocate(*advertisedRef);
	free(advertisedRef);
	advertisedRef = NULL;
    }
}


void disconnect_client(bip_peer_t *peer) {
    if (peer == NULL) return;
    if (peer->net == NULL) return;
    if (peer->net->socket != -1) close(peer->net->socket);
    peer->net->socket = -1;
}


void free_peer(void *peer_as_void) {
    bip_peer_t *peer = peer_as_void;
    if (peer == NULL) return;
    disconnect_client(peer);
    bip_peer_free(peer);
}


// clean up the clients hash table
void cleanup_clients(void *unused) {
    g_hash_table_unref(clients);
}


void *cal_server_mdnssd_bip_function(void *this_as_voidp) {
    this = this_as_voidp;

    TXTRecordRef txt_ref;
    DNSServiceErrorType error;


    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    // Shutup annoying nag message on Linux.
    setenv("AVAHI_COMPAT_NOWARN", "1", 1);


    clients = g_hash_table_new_full(g_str_hash, g_str_equal, free, free_peer);
    pthread_cleanup_push(cleanup_clients, NULL)


    advertisedRef = malloc(sizeof(DNSServiceRef));
    if (advertisedRef == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "server thread: out of memory!");
        return NULL;
    }

    TXTRecordCreate(&txt_ref, 0, NULL);

#if 0
    for (i = 0; i < peer->num_unicast_addresses; i ++) {
        char key[100];

        sprintf(key, "unicast-address-%d", i);
        error = TXTRecordSetValue ( 
            &txt_ref,                          // TXTRecordRef *txtRecord, 
            key,                               // const char *key, 
            strlen(peer->unicast_address[i]),  // uint8_t valueSize, /* may be zero */
            peer->unicast_address[i]           // const void *value /* may be NULL */
        );  

        if (error != kDNSServiceErr_NoError) {
            free(advertisedRef);
            advertisedRef = NULL;
            TXTRecordDeallocate(&txt_ref);
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "dnssd: Error registering service: %d", error);
            return 0;
        }
    }
#endif

    error = DNSServiceRegister(
        advertisedRef,                        // DNSServiceRef *sdRef
        0,                                    // DNSServiceFlags flags
        0,                                    // uint32_t interfaceIndex
        this->name,                           // const char *name
        CAL_MDNSSD_BIP_SERVICE_NAME,          // const char *regtype
        "",                                   // const char *domain
        NULL,                                 // const char *host
        htons(this->port),                    // uint16_t port (in network byte order)
        TXTRecordGetLength(&txt_ref),         // uint16_t txtLen
        TXTRecordGetBytesPtr(&txt_ref),       // const void *txtRecord
        register_callback,                    // DNSServiceRegisterReply callBack
        NULL                                  // void *context
    );

    if (error != kDNSServiceErr_NoError) {
        free(advertisedRef);
        advertisedRef = NULL;
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "server thread: Error registering service: %d", error);
        return NULL;
    }

    pthread_cleanup_push(cleanup_advertisedRef, NULL);


    while (1) {
        fd_set readers;
        int max_fd;
        int r;

        FD_ZERO(&readers);
        max_fd = -1;

        FD_SET(cal_server_mdnssd_bip_fds_from_user[0], &readers);
        max_fd = Max(max_fd, cal_server_mdnssd_bip_fds_from_user[0]);

        FD_SET(this->socket, &readers);
        max_fd = Max(max_fd, this->socket);


        // each client that's connected to us might want to say something
        {
            GHashTableIter iter;
            const char *name;
            bip_peer_t *peer;

            g_hash_table_iter_init (&iter, clients);
            while (g_hash_table_iter_next(&iter, (gpointer)&name, (gpointer)&peer)) {
                if (peer->net == NULL) continue;
                if (peer->net->socket == -1) continue;
                FD_SET(peer->net->socket, &readers);
                max_fd = Max(max_fd, peer->net->socket);
            }
        }


        // block until there's something to do
        r = select(max_fd + 1, &readers, NULL, NULL, NULL);

        if (FD_ISSET(cal_server_mdnssd_bip_fds_from_user[0], &readers)) {
            read_from_user();
        }

        if (FD_ISSET(this->socket, &readers)) {
            accept_connection(this);
        }


        // see if any of our clients said anything
        {
            GHashTableIter iter;
            const char *name;
            bip_peer_t *peer;

            g_hash_table_iter_init (&iter, clients);
            while (g_hash_table_iter_next(&iter, (gpointer)&name, (gpointer)&peer)) {
                if (peer->net == NULL) continue;
                if (peer->net->socket == -1) continue;
                if (FD_ISSET(peer->net->socket, &readers)) {
                    read_from_client(name, peer);
                    // read_from_client can disconnect the client, so we need to stop iterating over it now
                    break;
                }
            }
        }

    }

    // 
    // NOT REACHED
    //

    pthread_cleanup_pop(0);  // cleanup_advertisedRef
    pthread_cleanup_pop(0);  // cleanup_clients
}

