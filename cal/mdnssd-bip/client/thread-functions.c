
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




static GPtrArray *known_peers;

static GPtrArray *connected_publishers;




static cal_peer_t *find_peer_by_name(const char *name) {
    int i;

    for (i = 0; i < known_peers->len; i ++) {
        cal_peer_t *peer = g_ptr_array_index(known_peers, i);
        if (strcmp(peer->name, name) == 0) return peer;
    }

    return NULL;
}


static cal_peer_t *find_peer_by_ptr(const cal_peer_t *peer) {
    int i;

    for (i = 0; i < known_peers->len; i ++) {
        if (g_ptr_array_index(known_peers, i) == peer) return (cal_peer_t *)peer;
    }

    return NULL;
}


void disconnect_server(cal_peer_t *peer) {
    if (peer->as.ipv4.socket != -1) {
        close(peer->as.ipv4.socket);
    }
    peer->as.ipv4.socket = -1;
    g_ptr_array_remove_fast(connected_publishers, peer);
}




static int connect_to_peer(cal_peer_t *peer) {
    int s;
    int r;

    struct addrinfo ai_hints;
    struct addrinfo *ai;


    if (peer->addressing_scheme != CAL_AS_IPv4) {
        fprintf(stderr, ID "connect_to_peer(): peer '%s' has unknown addressing scheme %d\n", peer->name, peer->addressing_scheme);
        return -1;
    }

    if (peer->as.ipv4.socket >= 0) return peer->as.ipv4.socket;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        fprintf(stderr, ID "connect_to_peer(): error making socket: %s\n", strerror(errno));
        return -1;
    }

    memset(&ai_hints, 0, sizeof(ai_hints));
    ai_hints.ai_family = AF_INET;  // IPv4
    ai_hints.ai_socktype = SOCK_STREAM;  // TCP
    r = getaddrinfo(peer->as.ipv4.hostname, NULL, &ai_hints, &ai);
    if (r != 0) {
        fprintf(stderr, ID "connect_to_peer(): error with getaddrinfo(\"%s\", ...): %s", peer->as.ipv4.hostname, gai_strerror(r));
        return -1;
    }
    if (ai == NULL) {
        fprintf(stderr, ID "connect_to_peer(): no results from getaddrinfo(\"%s\", ...)", peer->as.ipv4.hostname);
        return -1;
    }

    ((struct sockaddr_in *)ai->ai_addr)->sin_port = htons(peer->as.ipv4.port);

    r = connect(s, ai->ai_addr, ai->ai_addrlen);
    if (r != 0) {
        struct sockaddr_in *sin = (struct sockaddr_in *)ai->ai_addr;
        fprintf(
            stderr,
            ID
            "connect_to_peer(): error connecting to peer '%s' at %s (%s): %s\n",
            peer->name,
            cal_peer_address_to_str(peer),
            inet_ntoa(sin->sin_addr),
            strerror(errno)
        );
        return -1;
    }

    peer->as.ipv4.socket = s;

    g_ptr_array_add(connected_publishers, peer);

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
            if (find_peer_by_ptr(event->peer) == NULL) {
                fprintf(stderr, ID "read_from_user: unknown peer pointer passed in, dropping outgoing Message event\n");
                return;
            }
            r = connect_to_peer(event->peer);
            if (r < 0) {
                return;
            }
            bip_send_message(event->peer, BIP_MSG_TYPE_MESSAGE, event->msg.buffer, event->msg.size);
            // FIXME: bip_sendto might not have worked, we need to report to the user or retry or something
            event->peer = NULL;  // the peer is still connected, dont free it yet
            break;
        }

        case CAL_EVENT_SUBSCRIBE: {
            if (find_peer_by_ptr(event->peer) == NULL) {
                fprintf(stderr, ID "read_from_user: unknown peer pointer passed in, dropping outgoing Subscribe event\n");
                return;
            }
            r = connect_to_peer(event->peer);
            if (r < 0) {
                return;
            }
            bip_send_message(event->peer, BIP_MSG_TYPE_SUBSCRIBE, event->msg.buffer, event->msg.size);
            // FIXME: bip_sendto might not have worked, we need to report to the user or retry or something
            event->peer = NULL;  // the peer is still connected, dont free it yet
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
static void read_from_publisher(cal_peer_t *peer) {
    cal_event_t *event;
    int r;
    int payload_size;

    r = bip_read_from_peer(peer);
    if (r < 0) {
        disconnect_server(peer);
        return;
    }

    // message is not here yet, wait for more to come in
    if (r == 0) return;


    // 
    // exactly one message is in the peer's buffer, handle it now
    //


    payload_size = ntohl(*(uint32_t*)&peer->buffer[BIP_MSG_HEADER_SIZE_OFFSET]);

    event = cal_event_new(CAL_EVENT_MESSAGE);
    if (event == NULL) {
        fprintf(stderr, ID "read_from_publisher(): out of memory!\n");
        peer->index = 0;
        return;
    }

    event->peer = peer;
    event->msg.buffer = malloc(payload_size);
    if (event->msg.buffer == NULL) {
        cal_event_free(event);
        fprintf(stderr, ID "read_from_publisher(): out of memory!\n");
        peer->index = 0;
        return;
    }

    memcpy(event->msg.buffer, &peer->buffer[BIP_MSG_HEADER_SIZE], payload_size);

    peer->index = 0;

    event->msg.size = payload_size;

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
    cal_peer_t *peer = event->peer;


    // remove this service_ref from the list
    DNSServiceRefDeallocate(sc->service_ref);
    service_list = g_slist_remove(service_list, sc);
    free(sc);

    if (errorCode != kDNSServiceErr_NoError) {
        fprintf(stderr, ID "resolve_callback: Error returned from resolve: %d\n", errorCode);
        cal_event_free(event);
        return;
    }

    peer->as.ipv4.port = ntohs(port);
    peer->as.ipv4.hostname = strdup(hosttarget);
    if (peer->as.ipv4.hostname == NULL) {
        fprintf(stderr, ID "resolve_callback: out of memory\n");
        cal_event_free(event);
        return;
    }

    // the peer is our responsibility, remember it for later
    g_ptr_array_add(known_peers, peer);

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

        event->peer = cal_peer_new(name);
        if (event->peer == NULL) {
            fprintf(stderr, ID "browse_callback: out of memory!  dropping this event!\n");
            cal_event_free(event);
            return;
        }

        cal_peer_set_addressing_scheme(event->peer, CAL_AS_IPv4);


        sc = malloc(sizeof(struct cal_client_mdnssd_bip_service_context));
        if (sc == NULL) {
            fprintf(stderr, ID "browse_callback: out of memory!  dropping this joining peer!\n");
            cal_event_free(event);
            return;
        }

        sc->event = event;

        // Now create a resolve call to fill out the rest of the cal_peer_t
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

        event = cal_event_new(CAL_EVENT_LEAVE);
        if (event == NULL) {
            fprintf(stderr, ID "browse_callback: out of memory!  dropping this event!\n");
            return;
        }

        event->peer = find_peer_by_name(name);
        if (event->peer == NULL) {
            fprintf(stderr, ID "browse_callback: got a Leave event for an unknown peer (%s)!  dropping this event!\n", name);
            cal_event_free(event);
            return;
        }

        g_ptr_array_remove_fast(known_peers, event->peer);

        // if we were connecte to this peer, we're not any more...
        disconnect_server(event->peer);

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


// clean up the known-peers list
void cleanup_known_peers(void *unused) {
    while (known_peers->len > 0) {
        cal_peer_t *peer;
        peer = g_ptr_array_remove_index_fast(known_peers, 0);
        cal_peer_free(peer);
    }

    g_ptr_array_free(known_peers, 1);
}


// clean up the connected-publishers list
void cleanup_connected_publishers(void *unused) {
    while (connected_publishers->len > 0) {
        cal_peer_t *peer;
        peer = g_ptr_array_index(connected_publishers, 0);
        disconnect_server(peer);
    }

    g_ptr_array_free(connected_publishers, 1);
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

    known_peers = g_ptr_array_new();
    pthread_cleanup_push(cleanup_known_peers, NULL);

    connected_publishers = g_ptr_array_new();
    pthread_cleanup_push(cleanup_connected_publishers, NULL);


    while (1) {
        fd_set readers;
        int max_fd;
        int r;
        int i;
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
        for (i = 0; i < connected_publishers->len; i ++) {
            cal_peer_t *peer = g_ptr_array_index(connected_publishers, i);
            FD_SET(peer->as.ipv4.socket, &readers);
            max_fd = Max(max_fd, peer->as.ipv4.socket);
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
        for (i = 0; i < connected_publishers->len; i ++) {
            cal_peer_t *peer = g_ptr_array_index(connected_publishers, i);
            if (FD_ISSET(peer->as.ipv4.socket, &readers)) {
                read_from_publisher(peer);

                // read_from_publisher can change the connected_publishers
                // list, so we need to stop iterating over it now
                break;
            }
        }
    }

    //
    // NOT REACHED!
    //

    pthread_cleanup_pop(0);  // don't execute cleanup_connected_publishers
    pthread_cleanup_pop(0);  // don't execute cleanup_known_peers
    pthread_cleanup_pop(0);  // don't execute cleanup_service_list
}

