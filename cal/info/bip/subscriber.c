
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

#include "cal.h"
#include "bip.h"


static pthread_t *cal_i_bip_subscriber_thread = NULL;

static int cal_i_bip_subscriber_fds_to_user[2];
static int cal_i_bip_subscriber_fds_from_user[2];

static  GPtrArray *bip_connected_publishers;




static int bip_connect_to_peer(cal_peer_t *peer) {
    int s;
    int r;

    struct addrinfo ai_hints;
    struct addrinfo *ai;


    if (peer->addressing_scheme != CAL_AS_IPv4) {
        fprintf(stderr, "bip: bip_connect_to_peer(): peer '%s' has unknown addressing scheme %d\n", peer->name, peer->addressing_scheme);
        return -1;
    }

    if (peer->as.ipv4.socket >= 0) return peer->as.ipv4.socket;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        fprintf(stderr, "bip: bip_connect_to_peer(): error making socket: %s\n", strerror(errno));
        return -1;
    }

    memset(&ai_hints, 0, sizeof(ai_hints));
    ai_hints.ai_family = AF_INET;  // IPv4
    ai_hints.ai_socktype = SOCK_STREAM;  // TCP
    r = getaddrinfo(peer->as.ipv4.hostname, NULL, &ai_hints, &ai);
    if (r != 0) {
        fprintf(stderr, "bip: bip_connect_to_peer(): error with getaddrinfo(\"%s\", ...): %s", peer->as.ipv4.hostname, gai_strerror(r));
        return -1;
    }
    if (ai == NULL) {
        fprintf(stderr, "bip: bip_connect_to_peer(): no results from getaddrinfo(\"%s\", ...)", peer->as.ipv4.hostname);
        return -1;
    }

    ((struct sockaddr_in *)ai->ai_addr)->sin_port = htons(peer->as.ipv4.port);

    r = connect(s, ai->ai_addr, ai->ai_addrlen);
    if (r != 0) {
        struct sockaddr_in *sin = (struct sockaddr_in *)ai->ai_addr;
        fprintf(
            stderr,
            "bip: bip_connect_to_peer(): error connecting to peer '%s' at %s (%s): %s\n",
            peer->name,
            cal_peer_address_to_str(peer),
            inet_ntoa(sin->sin_addr),
            strerror(errno)
        );
        return -1;
    }

    peer->as.ipv4.socket = s;

    g_ptr_array_add(bip_connected_publishers, peer);

    return s;
}




static int bip_subscriber_thread_sendto(cal_peer_t *peer, void *msg, int size) {
    int r;

    r = bip_connect_to_peer(peer);
    if (r < 0) {
        return -1;
    }

    printf("sending \"%s\" (%d bytes) to %s\n", (char *)msg, size, peer->name);

    return write(peer->as.ipv4.socket, msg, size);
}




// this function is called by the subscriber thread when the user app wants to tell it something
void bip_subscriber_read_from_user(void) {
    cal_event_t *event;
    int r;

    r = read(cal_i_bip_subscriber_fds_from_user[0], &event, sizeof(event));
    if (r < 0) {
        printf("bip: error reading from user: %s\n", strerror(errno));
        return;
    } else if (r != sizeof(event)) {
        printf("bip: short read from user\n");
        return;
    }

    switch (event->type) {
        case CAL_EVENT_MESSAGE: {
            bip_subscriber_thread_sendto(event->peer, event->msg.buffer, event->msg.size);
            break;
        }

        default: {
            fprintf(stderr, "bip: unknown event %d from user\n", event->type);
            break;
        }
    }
}




// this function is called by the subscriber thread when a connected publisher has something to say
static void bip_subscriber_read_from_publisher(cal_peer_t *peer) {
    cal_event_t *event;
    int r;

    event = cal_event_new(CAL_EVENT_MESSAGE);
    if (event == NULL) {
        fprintf(stderr, "bip_subscriber_read_from_publisher: out of memory!\n");
        return;
    }

    event->peer = peer;
    event->msg.buffer = malloc(BIP_MSG_BUFFER_SIZE);
    if (event->msg.buffer == NULL) {
        cal_event_free(event);
        fprintf(stderr, "bip_subscriber_read_from_publisher: out of memory!\n");
        return;
    }

    r = read(peer->as.ipv4.socket, event->msg.buffer, BIP_MSG_BUFFER_SIZE);
    if (r < 0) {
        printf("bip: error reading from peer %s: %s\n", peer->name, strerror(errno));
        return;
    }

    event->msg.size = r;

    r = write(cal_i_bip_subscriber_fds_to_user[1], &event, sizeof(event));
    if (r < 0) {
        fprintf(stderr, "bip_subscriber_read_from_publisher(): error writing to user thread!!");
    } else if (r < sizeof(event)) {
        fprintf(stderr, "bip_subscriber_read_from_publisher(): short write to user thread!!");
    }
}




//
// this function gets run by the thread started by init_subscriber()
// canceled by cancel_subscriber()
//

void *cal_i_bip_subscriber_function(void *arg) {
    // close(cal_i_bip_subscriber_fds_to_user[0]);
    // close(cal_i_bip_subscriber_fds_from_user[1]);

    bip_connected_publishers = g_ptr_array_new();

    while (1) {
        fd_set readers;
        int max_fd;
        int r;
        int i;

        FD_ZERO(&readers);
        max_fd = -1;

        FD_SET(cal_i_bip_subscriber_fds_from_user[0], &readers);
        max_fd = Max(max_fd, cal_i_bip_subscriber_fds_from_user[0]);

        for (i = 0; i < bip_connected_publishers->len; i ++) {
            cal_peer_t *peer = g_ptr_array_index(bip_connected_publishers, i);
            FD_SET(peer->as.ipv4.socket, &readers);
            max_fd = Max(max_fd, peer->as.ipv4.socket);
        }

        // block until there's something to do
        r = select(max_fd + 1, &readers, NULL, NULL, NULL);

        if (FD_ISSET(cal_i_bip_subscriber_fds_from_user[0], &readers)) {
            bip_subscriber_read_from_user();
        }

        for (i = 0; i < bip_connected_publishers->len; i ++) {
            cal_peer_t *peer = g_ptr_array_index(bip_connected_publishers, i);
            if (FD_ISSET(peer->as.ipv4.socket, &readers)) {
                bip_subscriber_read_from_publisher(peer);
            }
        }
    }
}


int bip_init_subscriber(void (*callback)(cal_event_t *event)) {
    int r;

    // create the pipe for passing events back to the user
    r = pipe(cal_i_bip_subscriber_fds_to_user);
    if (r < 0) {
        printf("bip init_subscriber: error making to-user pipe: %s\n", strerror(errno));
        goto fail0;
    }

    // create the pipe for getting subscription requests from the user
    r = pipe(cal_i_bip_subscriber_fds_from_user);
    if (r < 0) {
        printf("bip init_subscriber: error making from-user pipe: %s\n", strerror(errno));
        goto fail1;
    }

    cal_i_bip_subscriber_thread = (pthread_t *)malloc(sizeof(pthread_t));
    if (cal_i_bip_subscriber_thread == NULL) {
        printf("bip init_subscriber: cannot allocate memory for subscriber thread: %s\n", strerror(errno));
        goto fail2;
    }

    // record the user's callback function
    cal_i.subscriber_callback = callback;

    // start the subscriber thread to talk to the peers
    r = pthread_create(cal_i_bip_subscriber_thread, NULL, cal_i_bip_subscriber_function, NULL);
    if (r != 0) {
        printf("dnssd subscribe_peer_list: cannot create browser thread: %s\n", strerror(errno));
        goto fail3;
    }

    // close(cal_i_bip_subscriber_fds_to_user[1]);
    // close(cal_i_bip_subscriber_fds_from_user[0]);

    // the from_user fd is written by bip.read(), the user doesnt need to know it

    return cal_i_bip_subscriber_fds_to_user[0];


fail3:
    free(cal_i_bip_subscriber_thread);
    cal_i_bip_subscriber_thread = NULL;
    cal_i.subscriber_callback = NULL;

fail2:
    close(cal_i_bip_subscriber_fds_from_user[0]);
    close(cal_i_bip_subscriber_fds_from_user[1]);

fail1:
    close(cal_i_bip_subscriber_fds_to_user[0]);
    close(cal_i_bip_subscriber_fds_to_user[1]);

fail0:
    return -1;
}




void bip_cancel_subscriber(void) {
    if (cal_i_bip_subscriber_thread != NULL) {
        int r;

        r = pthread_cancel(*cal_i_bip_subscriber_thread);
        if (r != 0) {
            printf("cal_i_bip_cancel_subscriber: error canceling browser thread: %s\n", strerror(errno));
            return;
        } else {
            pthread_join(*cal_i_bip_subscriber_thread, NULL);
            free(cal_i_bip_subscriber_thread);
            cal_i_bip_subscriber_thread = NULL;
        }
    }

    close(cal_i_bip_subscriber_fds_to_user[0]);
    close(cal_i_bip_subscriber_fds_to_user[1]);
    close(cal_i_bip_subscriber_fds_from_user[0]);
    close(cal_i_bip_subscriber_fds_from_user[1]);

    cal_i.subscriber_callback = NULL;
}




void bip_subscribe(cal_peer_t *peer, char *topic) {
#if 0  // FIXME: need a Subscribe event
    bip_subscription_request_t *req;
    int r;

    req = (bip_subscription_request_t *)malloc(sizeof(bip_subscription_request_t));
    if (req == NULL) {
        printf("out of memory!\n");
        return;
    } 

    // FIXME
    req->peer = peer;
    req->topic = strdup(topic);

    r = write(cal_i_bip_subscriber_fds_from_user[1], &req, sizeof(bip_subscription_request_t*));
    if (r < 0) {
        printf("bip_subscribe: error writing subscription request: %s\n", strerror(errno));
    } else if (r != sizeof(bip_subscription_request_t*)) {
        printf("bip_subscribe: short write of subscription request!\n");
    }
#endif
}




// this function is called by the user app when some published data has come in
int bip_subscriber_read(void) {
    cal_event_t *event;
    int r;

    r = read(cal_i_bip_subscriber_fds_to_user[0], &event, sizeof(event));
    if (r < 0) {
        printf("error in bip_subscriber_read(): %s\n", strerror(errno));
        return 0;
    } else if (r != sizeof(event)) {
        printf("bip_subscriber_read(): short read from bip subscriber thread\n");
        return 0;
    }

    cal_i.subscriber_callback(event);

    // FIXME memory leak

    return 1;
}




int bip_subscriber_sendto(cal_peer_t *peer, void *msg, int size) {
    int r;
    cal_event_t *event;

    event = cal_event_new(CAL_EVENT_MESSAGE);
    if (event == NULL) {
        return -1;
    }

    event->peer = peer;
    event->msg.buffer = msg;
    event->msg.size = size;

    r = write(cal_i_bip_subscriber_fds_from_user[1], &event, sizeof(event));
    if (r < 0) {
        fprintf(stderr, "bip_subscriber_sendto(): error writing to subscriber thread!!");
        return -1;
    }
    if (r < sizeof(event)) {
        fprintf(stderr, "bip_subscriber_sendto(): short write to subscriber thread!!");
        return -1;
    }

    return 0;
}

