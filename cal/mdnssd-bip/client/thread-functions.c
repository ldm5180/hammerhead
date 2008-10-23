
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
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




bip_peer_t *get_peer_by_name(const char *peer_name) {
    char *hash_key;
    bip_peer_t *peer;

    peer = g_hash_table_lookup(peers, peer_name);
    if (peer != NULL) return peer;

    peer = calloc(1, sizeof(bip_peer_t));
    if (peer == NULL) {
        fprintf(stderr, ID "get_peer_by_name: out of memory\n");
        return NULL;
    };

    hash_key = strdup(peer_name);
    if (hash_key == NULL) {
        fprintf(stderr, ID "get_peer_by_name: out of memory\n");
        free(peer);
        return NULL;
    };

    g_hash_table_insert(peers, hash_key, peer);

    return peer;
}


void peer_leaves(bip_peer_t *peer) {
    if (peer == NULL) return;
    bip_net_free(peer->net);
    peer->net = NULL;
}


void disconnect_server(bip_peer_t *peer) {
    if (peer == NULL) return;
    if (peer->net == NULL) return;
    if (peer->net->socket != -1) close(peer->net->socket);
    peer->net->socket = -1;
}


int bip_client_send_message(const char *peer_name, const bip_peer_t *peer, uint8_t msg_type, const void *msg, uint32_t size) {
    int r;

    uint32_t msg_size;

    if (peer->net == NULL) return -1;
    if (peer->net->socket == -1) return -1;

    printf("bip_send_message: sending \"%s\" (%d bytes) to %s\n", (char *)msg, size, peer_name);

    msg_type = msg_type;
    msg_size = htonl(size);

    // FIXME: this should be one write

    r = write(peer->net->socket, &msg_type, sizeof(msg_type));
    if (r != sizeof(msg_type)) {
        return -1;
    }

    r = write(peer->net->socket, &msg_size, sizeof(msg_size));
    if (r != sizeof(msg_size)) {
        return -1;
    }

    if (size == 0) return 0;

    r = write(peer->net->socket, msg, size);
    if (r != sizeof(msg_size)) {
        return -1;
    }

    return 0;
}


int bip_client_read_from_peer(const char *peer_name, bip_peer_t *peer) {
    int r;
    int max_bytes_to_read;
    int payload_size;


    if (peer->net == NULL) return -1;
    if (peer->net->socket == -1) return -1;

    if (peer->net->index < BIP_MSG_HEADER_SIZE) {
        max_bytes_to_read = BIP_MSG_HEADER_SIZE - peer->net->index;
        r = read(peer->net->socket, &peer->net->buffer[peer->net->index], max_bytes_to_read);
        if (r < 0) {
            fprintf(stderr, "bip_read_from_peer(): error reading from peer %s: %s\n", peer_name, strerror(errno));
            return -1;
        } else if (r == 0) {
            fprintf(stderr, "bip_read_from_peer(): peer %s disconnects\n", peer_name);
            return -1;
        }

        peer->net->index += r;

        if (peer->net->index < BIP_MSG_HEADER_SIZE) return 0;
    }

    payload_size = ntohl(*(uint32_t *)&peer->net->buffer[BIP_MSG_HEADER_SIZE_OFFSET]);

    max_bytes_to_read = (payload_size + BIP_MSG_HEADER_SIZE) - peer->net->index;
    if (max_bytes_to_read > 0) {
        r = read(peer->net->socket, &peer->net->buffer[peer->net->index], max_bytes_to_read);
        if (r < 0) {
            fprintf(stderr, "bip_read_from_peer(): error reading from peer %s: %s\n", peer_name, strerror(errno));
            return -1;
        } else if (r == 0) {
            fprintf(stderr, "bip_read_from_peer(): peer %s disconnects\n", peer_name);
            return -1;
        }

        peer->net->index += r;

        if (r != max_bytes_to_read) return 0;
    }

    // yay a packet is in the peer's buffer!
    return 1;
}





static int connect_to_peer(const char *peer_name, bip_peer_t *peer) {
    int s;
    int r;

    struct addrinfo ai_hints;
    struct addrinfo *ai;


    if (peer->net == NULL) {
        fprintf(stderr, ID "connect_to_peer: peer '%s' has no known network address\n", peer_name);
        return -1;
    }

    if (peer->net->socket >= 0) return peer->net->socket;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        fprintf(stderr, ID "connect_to_peer(): error making socket: %s\n", strerror(errno));
        return -1;
    }

    memset(&ai_hints, 0, sizeof(ai_hints));
    ai_hints.ai_family = AF_INET;  // IPv4
    ai_hints.ai_socktype = SOCK_STREAM;  // TCP
    r = getaddrinfo(peer->net->hostname, NULL, &ai_hints, &ai);
    if (r != 0) {
        fprintf(stderr, ID "connect_to_peer(): error with getaddrinfo(\"%s\", ...): %s", peer->net->hostname, gai_strerror(r));
        return -1;
    }
    if (ai == NULL) {
        fprintf(stderr, ID "connect_to_peer(): no results from getaddrinfo(\"%s\", ...)", peer->net->hostname);
        return -1;
    }

    ((struct sockaddr_in *)ai->ai_addr)->sin_port = htons(peer->net->port);

    r = connect(s, ai->ai_addr, ai->ai_addrlen);
    if (r != 0) {
        struct sockaddr_in *sin = (struct sockaddr_in *)ai->ai_addr;
        fprintf(
            stderr,
            ID
            "connect_to_peer(): error connecting to peer '%s' at %s:%hu (%s): %s\n",
            peer_name,
            peer->net->hostname,
            peer->net->port,
            inet_ntoa(sin->sin_addr),
            strerror(errno)
        );
        return -1;
    }

    peer->net->socket = s;

    return s;
}




// this function is called by the thread main function when the user thread wants to tell it something
static void read_from_user(void) {
    cal_event_t *event;
    int r;

    r = read(cal_client_mdnssd_bip_fds_from_user[0], &event, sizeof(event));
    if (r < 0) {
        fprintf(stderr, ID "read_from_user(): error reading from user: %s\n", strerror(errno));
        return;
    } else if (r != sizeof(event)) {
        fprintf(stderr, ID "read_from_user(): short read from user\n");
        return;
    }

    switch (event->type) {
        case CAL_EVENT_MESSAGE: {
            bip_peer_t *peer;

            peer = get_peer_by_name(event->peer_name);
            if (peer == NULL) return;

            r = connect_to_peer(event->peer_name, peer);
            if (r < 0) return;

            bip_client_send_message(event->peer_name, peer, BIP_MSG_TYPE_MESSAGE, event->msg.buffer, event->msg.size);
            // FIXME: bip_sendto might not have worked, we need to report to the user or retry or something

            break;
        }


        case CAL_EVENT_SUBSCRIBE: {
            bip_peer_t *peer;

            peer = get_peer_by_name(event->peer_name);
            if (peer == NULL) return;

            r = connect_to_peer(event->peer_name, peer);
            if (r < 0) return;

            bip_client_send_message(event->peer_name, peer, BIP_MSG_TYPE_SUBSCRIBE, event->topic, strlen(event->topic) + 1);
            // FIXME: bip_sendto might not have worked, we need to report to the user or retry or something

            break;
        }

        default: {
            fprintf(stderr, ID "read_from_user(): unknown event %d from user\n", event->type);
            return;
        }
    }

    cal_event_free(event);
}




// this function is called by the thread main function when a connected publisher has something to say
static void read_from_publisher(const char *peer_name, bip_peer_t *peer) {
    cal_event_t *event;
    int r;
    int payload_size;

    r = bip_client_read_from_peer(peer_name, peer);
    if (r < 0) {
        disconnect_server(peer);
        return;
    }

    // message is not here yet, wait for more to come in
    if (r == 0) return;


    // 
    // exactly one message is in the peer's buffer, handle it now
    //


    payload_size = ntohl(*(uint32_t*)&peer->net->buffer[BIP_MSG_HEADER_SIZE_OFFSET]);

    switch (peer->net->buffer[BIP_MSG_HEADER_TYPE_OFFSET]) {
        case BIP_MSG_TYPE_MESSAGE: {
            event = cal_event_new(CAL_EVENT_MESSAGE);
            if (event == NULL) {
                fprintf(stderr, ID "read_from_publisher(): out of memory!\n");
                peer->net->index = 0;
                return;
            }

            event->peer_name = strdup(peer_name);
            if (event->peer_name == NULL) {
                cal_event_free(event);
                fprintf(stderr, ID "read_from_publisher: out of memory!\n");
                peer->net->index = 0;
                return;
            }

            event->msg.buffer = malloc(payload_size);
            if (event->msg.buffer == NULL) {
                cal_event_free(event);
                fprintf(stderr, ID "read_from_publisher(): out of memory!\n");
                peer->net->index = 0;
                return;
            }

            memcpy(event->msg.buffer, &peer->net->buffer[BIP_MSG_HEADER_SIZE], payload_size);

            event->msg.size = payload_size;

            break;
        }

        case BIP_MSG_TYPE_PUBLISH: {
            event = cal_event_new(CAL_EVENT_PUBLISH);
            if (event == NULL) {
                fprintf(stderr, ID "read_from_publisher(): out of memory!\n");
                peer->net->index = 0;
                return;
            }

            event->peer_name = strdup(peer_name);
            if (event->peer_name == NULL) {
                cal_event_free(event);
                fprintf(stderr, ID "read_from_publisher(): out of memory!\n");
                peer->net->index = 0;
                return;
            }

            event->msg.buffer = malloc(payload_size);
            if (event->msg.buffer == NULL) {
                cal_event_free(event);
                fprintf(stderr, ID "read_from_publisher(): out of memory!\n");
                peer->net->index = 0;
                return;
            }

            memcpy(event->msg.buffer, &peer->net->buffer[BIP_MSG_HEADER_SIZE], payload_size);

            event->msg.size = payload_size;

            break;
        }
    }


    peer->net->index = 0;

    r = write(cal_client_mdnssd_bip_fds_to_user[1], &event, sizeof(event));
    if (r < 0) {
        fprintf(stderr, ID "read_from_publisher(): error writing to user thread!!");
    } else if (r < sizeof(event)) {
        fprintf(stderr, ID "read_from_publisher(): short write to user thread!!");
    }
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

    char *peer_name;
    bip_peer_t *peer;


    // remove this service_ref from the list
    DNSServiceRefDeallocate(sc->service_ref);
    service_list = g_slist_remove(service_list, sc);
    free(sc);

    if (errorCode != kDNSServiceErr_NoError) {
        fprintf(stderr, ID "resolve_callback: Error returned from resolve: %d\n", errorCode);
        cal_event_free(event);
        return;
    }

    peer_name = strdup(event->peer_name);
    if (peer_name == NULL) {
        fprintf(stderr, ID "resolve_callback: out of memory\n");
        return;
    }

    peer = get_peer_by_name(peer_name);
    if (peer == NULL) {
        fprintf(stderr, ID "resolve_callback: out of memory\n");
        free(peer_name);
        return;
    }

    // FIXME: check that peer has no net

    peer->net = calloc(1, sizeof(bip_peer_network_info_t));
    if (peer->net == NULL) {
        fprintf(stderr, ID "resolve_callback: out of memory\n");
        free(peer_name);
        cal_event_free(event);
        return;
    }
    peer->net->socket = -1;  // not connected yet

    peer->net->port = ntohs(port);

    peer->net->hostname = strdup(hosttarget);
    if (peer->net->hostname == NULL) {
        fprintf(stderr, ID "resolve_callback: out of memory\n");
        cal_event_free(event);
        free(peer_name);
        free(peer->net);
        return;
    }

    // the event becomes the responsibility of the callback now, so they might leak memory but we're not
    r = write(cal_client_mdnssd_bip_fds_to_user[1], &event, sizeof(event));  // heh
    if (r < 0) {
        fprintf(stderr, ID "resolve_callback: error writing event: %s\n", strerror(errno));
    } else if (r != sizeof(event)) {
        fprintf(stderr, ID "resolve_callback: short write while writing event\n");
    }
} 




// 
// this function gets called whenever a service of our type, "_bionet._tcp", comes or goes
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
        fprintf(stderr, ID "browse_callback: Error returned from browse: %d\n", errorCode);
        return;
    }


    if (flags & kDNSServiceFlagsAdd) {
        struct cal_client_mdnssd_bip_service_context *sc;
        DNSServiceErrorType error;
        cal_event_t *event;

        event = cal_event_new(CAL_EVENT_JOIN);
        if (event == NULL) {
            fprintf(stderr, ID "browse_callback: out of memory!  dropping this event!\n");
            return;
        }

        event->peer_name = strdup(name);
        if (event->peer_name == NULL) {
            fprintf(stderr, ID "browse_callback: out of memory!  dropping this event!\n");
            cal_event_free(event);
            return;
        }


        sc = malloc(sizeof(struct cal_client_mdnssd_bip_service_context));
        if (sc == NULL) {
            fprintf(stderr, ID "browse_callback: out of memory!  dropping this joining peer!\n");
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
            fprintf(stderr, ID "browse_callabck: failed to start resolv service, dropping this joining peer\n");
            cal_event_free(event);
            free(sc);
            return;
        }

        // add this new service ref to the list
        // printf("adding this resolve from service ref list\n");
        service_list = g_slist_prepend(service_list, sc);
    } else {
        int r;
        cal_event_t *event;
        bip_peer_t *peer;

        peer = g_hash_table_lookup(peers, name);
        if (peer == NULL) return;  // strange, we haven't seen a Join for this peer, so ignore the Leave event

        disconnect_server(peer);
        peer_leaves(peer);

        event = cal_event_new(CAL_EVENT_LEAVE);
        if (event == NULL) {
            fprintf(stderr, ID "browse_callback: out of memory!  dropping this event!\n");
            return;
        }

        event->peer_name = strdup(name);
        if (event->peer_name == NULL) {
            fprintf(stderr, ID "browse_callback: out of memory!  dropping this event!\n");
            cal_event_free(event);
            return;
        }

        // the event and the peer become the responsibility of the callback now, so they might leak memory but we're not
        r = write(cal_client_mdnssd_bip_fds_to_user[1], &event, sizeof(event));  // heh
        if (r < 0) {
            fprintf(stderr, ID "browser_callback: error writing event: %s\n", strerror(errno));
        } else if (r != sizeof(event)) {
            fprintf(stderr, ID "browser_callback: short write while writing event\n");
        }
    }
}




// cancel all pending mDNS-SD service requests
void cleanup_service_list(void *unused) {
    GSList *ptr;

    printf("cleaning up\n");

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


void free_peer(void *peer_as_void) {
    bip_peer_t *peer = peer_as_void;

    if (peer == NULL) return;

    disconnect_server(peer);
    bip_peer_free(peer);
}


//
// this function gets run in the thread started by init()
// canceled by shutdown()
//

void *cal_client_mdnssd_bip_function(void *arg) {
    struct cal_client_mdnssd_bip_service_context *browse;
    DNSServiceErrorType error;

    // Shutup annoying nag message on Linux.
    setenv("AVAHI_COMPAT_NOWARN", "1", 1);

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    browse = malloc(sizeof(struct cal_client_mdnssd_bip_service_context));
    if (browse == NULL) {
        fprintf(stderr, ID "client thread: out of memory!\n");
        return NULL;
    }

    error = DNSServiceBrowse(
        &browse->service_ref,
        0,
        0,
	CAL_MDNSSD_BIP_SERVICE_NAME,
        NULL,  
        browse_callback,
        NULL
    );

    if (error != kDNSServiceErr_NoError) {
        free(browse);
	fprintf(stderr, ID "client thread: Error browsing for service: %d\n", error);
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
                if (peer->net == NULL) continue;
                if (peer->net->socket == -1) continue;
                FD_SET(peer->net->socket, &readers);
                max_fd = Max(max_fd, peer->net->socket);
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
                    fprintf(stderr, ID "client thread: Error processing service reference result: %d.\n", err);
                    sleep(1);
                }

                // since the service_list has changed, we should stop iterating over it
                break;
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
                if (peer->net == NULL) continue;
                if (peer->net->socket == -1) continue;
                if (FD_ISSET(peer->net->socket, &readers)) {
                    read_from_publisher(name, peer);
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
}

