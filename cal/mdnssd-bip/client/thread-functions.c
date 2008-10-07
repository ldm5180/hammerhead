
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




static  GPtrArray *connected_publishers;




static int bip_connect_to_peer(cal_peer_t *peer) {
    int s;
    int r;

    struct addrinfo ai_hints;
    struct addrinfo *ai;


    if (peer->addressing_scheme != CAL_AS_IPv4) {
        fprintf(stderr, ID "bip_connect_to_peer(): peer '%s' has unknown addressing scheme %d\n", peer->name, peer->addressing_scheme);
        return -1;
    }

    if (peer->as.ipv4.socket >= 0) return peer->as.ipv4.socket;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        fprintf(stderr, ID "bip_connect_to_peer(): error making socket: %s\n", strerror(errno));
        return -1;
    }

    memset(&ai_hints, 0, sizeof(ai_hints));
    ai_hints.ai_family = AF_INET;  // IPv4
    ai_hints.ai_socktype = SOCK_STREAM;  // TCP
    r = getaddrinfo(peer->as.ipv4.hostname, NULL, &ai_hints, &ai);
    if (r != 0) {
        fprintf(stderr, ID "bip_connect_to_peer(): error with getaddrinfo(\"%s\", ...): %s", peer->as.ipv4.hostname, gai_strerror(r));
        return -1;
    }
    if (ai == NULL) {
        fprintf(stderr, ID "bip_connect_to_peer(): no results from getaddrinfo(\"%s\", ...)", peer->as.ipv4.hostname);
        return -1;
    }

    ((struct sockaddr_in *)ai->ai_addr)->sin_port = htons(peer->as.ipv4.port);

    r = connect(s, ai->ai_addr, ai->ai_addrlen);
    if (r != 0) {
        struct sockaddr_in *sin = (struct sockaddr_in *)ai->ai_addr;
        fprintf(
            stderr,
            ID
            "bip_connect_to_peer(): error connecting to peer '%s' at %s (%s): %s\n",
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




static int bip_sendto(cal_peer_t *peer, void *msg, int size) {
    int r;

    r = bip_connect_to_peer(peer);
    if (r < 0) {
        return -1;
    }

    printf(ID "bip_sendto(): sending \"%s\" (%d bytes) to %s\n", (char *)msg, size, peer->name);

    return write(peer->as.ipv4.socket, msg, size);
}




// this function is called by the thread main function when the user thread wants to tell it something
static void bip_read_from_user(void) {
    cal_event_t *event;
    int r;

    r = read(cal_client_mdnssd_bip_fds_from_user[0], &event, sizeof(event));
    if (r < 0) {
        printf(ID "read_from_user(): error reading from user: %s\n", strerror(errno));
        return;
    } else if (r != sizeof(event)) {
        printf(ID "read_from_user(): short read from user\n");
        return;
    }

    switch (event->type) {
        case CAL_EVENT_MESSAGE: {
            bip_sendto(event->peer, event->msg.buffer, event->msg.size);
            break;
        }

        default: {
            fprintf(stderr, ID "read_from_user(): unknown event %d from user\n", event->type);
            break;
        }
    }
}




// this function is called by the thread main function when a connected publisher has something to say
static void bip_read_from_publisher(cal_peer_t *peer) {
    cal_event_t *event;
    int r;

    event = cal_event_new(CAL_EVENT_MESSAGE);
    if (event == NULL) {
        fprintf(stderr, ID "bip_read_from_publisher(): out of memory!\n");
        return;
    }

    event->peer = peer;
    event->msg.buffer = malloc(BIP_MSG_BUFFER_SIZE);
    if (event->msg.buffer == NULL) {
        cal_event_free(event);
        fprintf(stderr, ID "bip_read_from_publisher(): out of memory!\n");
        return;
    }

    r = read(peer->as.ipv4.socket, event->msg.buffer, BIP_MSG_BUFFER_SIZE);
    if (r < 0) {
        printf(ID "bip_read_from_publisher(): error reading from peer %s: %s\n", peer->name, strerror(errno));
        return;
    }

    event->msg.size = r;

    r = write(cal_client_mdnssd_bip_fds_to_user[1], &event, sizeof(event));
    if (r < 0) {
        fprintf(stderr, ID "bip_read_from_publisher(): error writing to user thread!!");
    } else if (r < sizeof(event)) {
        fprintf(stderr, ID "bip_read_from_publisher(): short write to user thread!!");
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
    cal_client_mdnssd_bip_service_list = g_slist_remove(cal_client_mdnssd_bip_service_list, sc);
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

    cal_event_t *event;


    if (errorCode != kDNSServiceErr_NoError) {
        fprintf(stderr, ID "browse_callback: Error returned from browse: %d\n", errorCode);
        return;
    }


    event = cal_event_new(CAL_EVENT_NONE);
    if (event == NULL) {
        fprintf(stderr, ID "browse_callback: out of memory!  dropping this event!\n");
        return;
    }

    // FIXME: might be a leave, in which case we should look up the peer, not make a new one
    event->peer = cal_peer_new(name);
    if (event->peer == NULL) {
        fprintf(stderr, ID "browse_callback: out of memory!  dropping this event!\n");
        cal_event_free(event);
        return;
    }

    cal_peer_set_addressing_scheme(event->peer, CAL_AS_IPv4);


    if (flags & kDNSServiceFlagsAdd) {
        struct cal_client_mdnssd_bip_service_context *sc;
        DNSServiceErrorType error;

        event->type = CAL_EVENT_JOIN;

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
        cal_client_mdnssd_bip_service_list = g_slist_prepend(cal_client_mdnssd_bip_service_list, sc);
    } else {
        int r;

        event->type = CAL_EVENT_LEAVE;

        // Do we really need to fillout all the fields in the cal_peer_t?
        // Probably not.  Plus it is now gone.  For now, just fill out
        // the name.

        // the event becomes the responsibility of the callback now, so they might leak memory but we're not
        r = write(cal_client_mdnssd_bip_fds_to_user[1], &event, sizeof(event));  // heh
        if (r < 0) {
            fprintf(stderr, ID "browser_callback: error writing event: %s\n", strerror(errno));
        } else if (r != sizeof(event)) {
            fprintf(stderr, ID "browser_callback: short write while writing event\n");
        }
    }
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

    cal_client_mdnssd_bip_service_list = g_slist_prepend(cal_client_mdnssd_bip_service_list, browse);
    
    connected_publishers = g_ptr_array_new();


    while (1) {
        fd_set readers;
        int max_fd;
        int r;
        int i;
        GSList *ptr;


        FD_ZERO(&readers);
        max_fd = -1;


        // each DNS-SD service request has its own fd
        for (ptr = cal_client_mdnssd_bip_service_list; ptr != NULL; ptr = ptr->next) {
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
        for (ptr = cal_client_mdnssd_bip_service_list; ptr != NULL; ptr = ptr->next) {
            struct cal_client_mdnssd_bip_service_context *sc = ptr->data;
            int fd = DNSServiceRefSockFD(sc->service_ref);

            if (FD_ISSET(fd, &readers)) {
                DNSServiceErrorType err;

                err = DNSServiceProcessResult(sc->service_ref);
                if (err != kDNSServiceErr_NoError) {
                    fprintf(stderr, ID "client thread: Error processing service reference result: %d.\n", err);
                    // FIXME
                    return NULL;
                }
                break;
            }
        }

        // see if the user thread said anything
        if (FD_ISSET(cal_client_mdnssd_bip_fds_from_user[0], &readers)) {
            bip_read_from_user();
        }

        // see if any of our servers said anything
        for (i = 0; i < connected_publishers->len; i ++) {
            cal_peer_t *peer = g_ptr_array_index(connected_publishers, i);
            if (FD_ISSET(peer->as.ipv4.socket, &readers)) {
                bip_read_from_publisher(peer);
                break;
            }
        }
    }
}


