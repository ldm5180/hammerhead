
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




static GPtrArray *clients = NULL;

static DNSServiceRef *advertisedRef = NULL;




static cal_peer_t *find_client_by_ptr(const cal_peer_t *peer) {
    int i;

    for (i = 0; i < clients->len; i ++) {
        if (g_ptr_array_index(clients, i) == peer) return (cal_peer_t *)peer;
    }

    return NULL;
}




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
        fprintf(stderr, ID "register_callback: Register callback returned %d\n", errorCode);
    }
}




static void read_from_user(void) {
    cal_event_t *event;
    int r;

    r = read(cal_server_mdnssd_bip_fds_from_user[0], &event, sizeof(event));
    if (r < 0) {
        fprintf(stderr, ID "read_from_user: error reading from user: %s\n", strerror(errno));
        return;
    } else if (r != sizeof(event)) {
        fprintf(stderr, ID "read_from_user: short read from user\n");
        return;
    }

    switch (event->type) {
        case CAL_EVENT_MESSAGE: {
            if (find_client_by_ptr(event->peer) == NULL) {
                fprintf(stderr, ID "read_from_user: unknown peer pointer passed in, dropping outgoing Message event\n");
                return;
            }
            bip_send_message(event->peer, BIP_MSG_TYPE_MESSAGE, event->msg.buffer, event->msg.size);
            // FIXME: bip_sendto might not have worked, we need to report to the user or retry or something
            event->peer = NULL;  // the peer is still connected, dont free it yet
            break;
        }

        default: {
            fprintf(stderr, ID "read_from_user(): unknown event %d from user\n", event->type);
            return;  // dont free events we dont understand
        }
    }

    cal_event_free(event);
}




static void accept_connection(void) {
    int r;
    int socket;
    struct sockaddr_in sin;
    socklen_t sin_len;

    cal_event_t *event;


    sin_len = sizeof(struct sockaddr);
    socket = accept(cal_server_mdnssd_bip_listening_socket, (struct sockaddr *)&sin, &sin_len);
    if (socket < 0) {
        fprintf(stderr, ID "accept_connection(): error accepting a connection: %s\n", strerror(errno));
        return;
    }

    event = cal_event_new(CAL_EVENT_CONNECT);
    if (event == NULL) {
        // an error has been logged already
        close(socket);
        return;
    }

    event->peer = cal_peer_new(NULL);
    if (event->peer == NULL) {
        // an error has been logged already
        cal_event_free(event);
        close(socket);
        return;
    }

    cal_peer_set_addressing_scheme(event->peer, CAL_AS_IPv4);
    event->peer->as.ipv4.socket = socket;
    event->peer->as.ipv4.port = ntohs(sin.sin_port);

    event->peer->as.ipv4.hostname = strdup(inet_ntoa(sin.sin_addr));
    if (event->peer->as.ipv4.hostname == NULL) {
        fprintf(stderr, ID "accept_connection(): out of memory\n");
        cal_event_free(event);
        close(socket);
        return;
    }

    g_ptr_array_add(clients, event->peer);

    r = write(cal_server_mdnssd_bip_fds_to_user[1], &event, sizeof(cal_event_t*));
    if (r < 0) {
        fprintf(stderr, ID "accept_connection(): error writing Connect event: %s\n", strerror(errno));
    } else if (r != sizeof(cal_event_t*)) {
        fprintf(stderr, ID "accept_connection(): short write of Connect event!\n");
    }
}




static void disconnect_peer(cal_peer_t *peer) {
    int r;
    cal_event_t *event;

    close(peer->as.ipv4.socket);
    peer->as.ipv4.socket = -1;

    g_ptr_array_remove_fast(clients, peer);

    event = cal_event_new(CAL_EVENT_DISCONNECT);
    if (event == NULL) {
        fprintf(stderr, ID "disconnect_peer(): out of memory\n");
        cal_peer_free(peer);
        return;
    }

    event->peer = peer;

    r = write(cal_server_mdnssd_bip_fds_to_user[1], &event, sizeof(cal_event_t*));
    if (r < 0) {
        fprintf(stderr, ID "disconnect_peer(): error writing Disconnect event: %s\n", strerror(errno));
    } else if (r != sizeof(cal_event_t*)) {
        fprintf(stderr, ID "disconnect_peer(): short write of Disconnect event!\n");
    }
}




// reads from the peer, sends a Message event to the user thread
// returns 0 on success, -1 on failure (in which case the peer is disconnected)
static int read_from_client(cal_peer_t *peer) {
    int r;
    int payload_size;
    cal_event_t *event;

    r = bip_read_from_peer(peer);
    if (r < 0) {
        disconnect_peer(peer);
        return -1;
    }

    // message is not here yet, wait for more to come in
    if (r == 0) return 0;


    // 
    // exactly one message is in the peer's buffer, handle it now
    //


    payload_size = ntohl(*(uint32_t*)&peer->buffer[BIP_MSG_HEADER_SIZE_OFFSET]);

    // the actual event type will be set below
    event = cal_event_new(CAL_EVENT_NONE);
    if (event == NULL) {
        fprintf(stderr, ID "read_from_client: out of memory!\n");
        peer->index = 0;
        return -1;
    }

    switch (peer->buffer[BIP_MSG_HEADER_TYPE_OFFSET]) {
        case BIP_MSG_TYPE_MESSAGE: {
            event->type = CAL_EVENT_MESSAGE;

            event->msg.buffer = malloc(payload_size);
            if (event->msg.buffer == NULL) {
                cal_event_free(event);
                fprintf(stderr, ID "read_from_client: out of memory!\n");
                peer->index = 0;
                return -1;
            }

            memcpy(event->msg.buffer, &peer->buffer[BIP_MSG_HEADER_SIZE], payload_size);
            event->msg.size = payload_size;

            peer->index = 0;

            break;
        }

        case BIP_MSG_TYPE_SUBSCRIBE: {
            event->type = CAL_EVENT_SUBSCRIBE;

            event->topic = malloc(payload_size);
            if (event->topic == NULL) {
                cal_event_free(event);
                fprintf(stderr, ID "read_from_client: out of memory!\n");
                peer->index = 0;
                return -1;
            }

            memcpy(event->topic, &peer->buffer[BIP_MSG_HEADER_SIZE], payload_size);

            peer->index = 0;
            break;
        }

        default: {
            fprintf(stderr, ID "read_from_client: dont know what to do with message type %d\n", peer->buffer[BIP_MSG_HEADER_TYPE_OFFSET]);
            cal_event_free(event);
            return -1;
        }
    }

    event->peer = peer;


    r = write(cal_server_mdnssd_bip_fds_to_user[1], &event, sizeof(event));
    if (r < 0) {
        fprintf(stderr, ID "read_from_client: error writing to user thread!!");
        return -1;
    } else if (r < sizeof(event)) {
        fprintf(stderr, ID "read_from_client: short write to user thread!!");
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


// clean up the clients list
void cleanup_clients(void *unused) {
    while (clients->len > 0) {
        cal_peer_t *peer;
        peer = g_ptr_array_remove_index_fast(clients, 0);
        close(peer->as.ipv4.socket);
        cal_peer_free(peer);
    }

    g_ptr_array_free(clients, 1);
}


void *cal_server_mdnssd_bip_function(void *peer_as_voidp) {
    cal_peer_t *this = peer_as_voidp;

    TXTRecordRef txt_ref;
    DNSServiceErrorType error;


    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    // Shutup annoying nag message on Linux.
    setenv("AVAHI_COMPAT_NOWARN", "1", 1);


    clients = g_ptr_array_new();
    pthread_cleanup_push(cleanup_clients, NULL)


    advertisedRef = malloc(sizeof(DNSServiceRef));
    if (advertisedRef == NULL) {
        fprintf(stderr, ID "server thread: out of memory!\n");
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
            fprintf(stderr, "dnssd: Error registering service: %d\n", error);
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
        htons(this->as.ipv4.port),            // uint16_t port (in network byte order)
        TXTRecordGetLength(&txt_ref),         // uint16_t txtLen
        TXTRecordGetBytesPtr(&txt_ref),       // const void *txtRecord
        register_callback,                    // DNSServiceRegisterReply callBack
        NULL                                  // void *context
    );

    if (error != kDNSServiceErr_NoError) {
        free(advertisedRef);
        advertisedRef = NULL;
        fprintf(stderr, ID "server thread: Error registering service: %d\n", error);
        return NULL;
    }

    pthread_cleanup_push(cleanup_advertisedRef, NULL);


    while (1) {
        int i;
        fd_set readers;
        int max_fd;
        int r;

        FD_ZERO(&readers);
        max_fd = -1;

        FD_SET(cal_server_mdnssd_bip_fds_from_user[0], &readers);
        max_fd = Max(max_fd, cal_server_mdnssd_bip_fds_from_user[0]);

        FD_SET(cal_server_mdnssd_bip_listening_socket, &readers);
        max_fd = Max(max_fd, cal_server_mdnssd_bip_listening_socket);


        // see if any peer has anything to say
        for (i = 0; i < clients->len; i ++) {
            cal_peer_t *peer = g_ptr_array_index(clients, i);
            int fd = peer->as.ipv4.socket;

            FD_SET(fd, &readers);
            max_fd = Max(max_fd, fd);
        }


        // block until there's something to do
        r = select(max_fd + 1, &readers, NULL, NULL, NULL);

        if (FD_ISSET(cal_server_mdnssd_bip_fds_from_user[0], &readers)) {
            read_from_user();
        }

        if (FD_ISSET(cal_server_mdnssd_bip_listening_socket, &readers)) {
            accept_connection();
        }

        // see if any peer has anything to say
        for (i = 0; i < clients->len; i ++) {
            cal_peer_t *peer = g_ptr_array_index(clients, i);
            int fd = peer->as.ipv4.socket;

            if (FD_ISSET(fd, &readers)) {
                if (read_from_client(peer) != 0) {
                    // the client disconnected, which probably reordered the clients array
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

