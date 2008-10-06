
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

#include "cal.h"
#include "bip.h"




static pthread_t *bip_publisher_thread = NULL;

static int bip_publisher_fds_to_user[2];
static int bip_publisher_fds_from_user[2];

static int bip_listening_socket = -1;

static GPtrArray *clients;




static void bip_publisher_disconnect_peer(cal_peer_t *peer) {
    int r;
    cal_event_t *event;

    close(peer->as.ipv4.socket);
    peer->as.ipv4.socket = -1;

    g_ptr_array_remove_fast(clients, peer);

    event = cal_event_new(CAL_EVENT_DISCONNECT);
    if (event == NULL) {
        fprintf(stderr, "bip_disconnect_peer(): out of memory\n");
        return;
    }

    event->peer = peer;

    r = write(bip_publisher_fds_to_user[1], &event, sizeof(cal_event_t*));
    if (r < 0) {
        printf("bip_publisher_disconnect_peer(): error writing Disconnect event: %s\n", strerror(errno));
    } else if (r != sizeof(cal_event_t*)) {
        printf("bip_publisher_disconnect_peer(): short write of Disconnect event!\n");
    }
}


static void bip_publisher_read_from_user(void) {
    printf("bip: user has something to say\n");
}


static void bip_publisher_accept_connection(void) {
    int r;
    int socket;
    struct sockaddr_in sin;
    socklen_t sin_len;

    cal_event_t *event;


    sin_len = sizeof(struct sockaddr);
    socket = accept(bip_listening_socket, (struct sockaddr *)&sin, &sin_len);
    if (socket < 0) {
        fprintf(stderr, "bip_publisher_accept_connection(): error accepting a connection: %s\n", strerror(errno));
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
        fprintf(stderr, "bip_publisher_accept_connection(): out of memory\n");
        cal_event_free(event);
        close(socket);
        return;
    }

    g_ptr_array_add(clients, event->peer);

    r = write(bip_publisher_fds_to_user[1], &event, sizeof(cal_event_t*));
    if (r < 0) {
        printf("bip_publisher_accept_connection(): error writing Connect event: %s\n", strerror(errno));
    } else if (r != sizeof(cal_event_t*)) {
        printf("bip_publisher_accept_connection(): short write of Connect event!\n");
    }
}


#define BIP_MSG_BUFFER_SIZE (1024)

static int bip_publisher_read_from_client(cal_peer_t *peer) {
    int r;
    cal_event_t *event;


    event = cal_event_new(CAL_EVENT_MESSAGE);
    if (event == NULL) {
        fprintf(stderr, "bip_publisher_read_from_client(): out of memory!\n");
        goto fail0;
    }

    event->msg.buffer = malloc(BIP_MSG_BUFFER_SIZE);
    if (event->msg.buffer == NULL) {
        fprintf(stderr, "bip_publisher_read_from_client(): out of memory!\n");
        goto fail1;
    }

    r = read(peer->as.ipv4.socket, event->msg.buffer, BIP_MSG_BUFFER_SIZE);
    if (r < 0) {
        fprintf(stderr, "bip_publisher_read_from_client(): error reading from client %s (%s): %s\n", peer->name, cal_peer_address_to_str(peer), strerror(errno));
        goto fail1;
    } else if (r == 0) {
        fprintf(stderr, "bip_publisher_read_from_client(): client %s (%s) disconnects\n", peer->name, cal_peer_address_to_str(peer));
        goto fail1;
    }

    event->msg.size = r;
    event->peer = peer;

    r = write(bip_publisher_fds_to_user[1], &event, sizeof(cal_event_t*));
    if (r < 0) {
        printf("bip_publisher_read_from_client(): error writing Message event: %s\n", strerror(errno));
        goto fail1;
    } else if (r != sizeof(cal_event_t*)) {
        printf("bip_publisher_read_from_client(): short write of Message event!\n");
        goto fail1;
    }

    return 0;

fail1:
    cal_event_free(event);

fail0:
    bip_publisher_disconnect_peer(peer);
    return -1;
}


static void *bip_publisher_function(void *arg) {
    clients = g_ptr_array_new();

    while (1) {
        int i;
        fd_set readers;
        int max_fd;
        int r;

        FD_ZERO(&readers);
        max_fd = -1;

        FD_SET(bip_publisher_fds_from_user[0], &readers);
        max_fd = Max(max_fd, bip_publisher_fds_from_user[0]);

        FD_SET(bip_listening_socket, &readers);
        max_fd = Max(max_fd, bip_listening_socket);


        // see if any peer has anything to say
        for (i = 0; i < clients->len; i ++) {
            cal_peer_t *peer = g_ptr_array_index(clients, i);
            int fd = peer->as.ipv4.socket;

            FD_SET(fd, &readers);
            max_fd = Max(max_fd, fd);
        }


        // block until there's something to do
        r = select(max_fd + 1, &readers, NULL, NULL, NULL);

        if (FD_ISSET(bip_publisher_fds_from_user[0], &readers)) {
            bip_publisher_read_from_user();
        }

        if (FD_ISSET(bip_listening_socket, &readers)) {
            bip_publisher_accept_connection();
        }

        // see if any peer has anything to say
        for (i = 0; i < clients->len; i ++) {
            cal_peer_t *peer = g_ptr_array_index(clients, i);
            int fd = peer->as.ipv4.socket;

            if (FD_ISSET(fd, &readers)) {
                if (bip_publisher_read_from_client(peer) != 0) {
                    // the client disconnected, which probably reordered the clients array
                    break;
                }
            }
        }
    }
}


int bip_init_publisher(cal_peer_t *this, void (*callback)(cal_event_t *event)) {
    int r;
    int sock;

    struct sockaddr_in my_address;
    socklen_t my_address_len;


    //
    // create the socket
    //

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("ERROR: cannot create TCP socket: %s\n", strerror(errno));
        return -1;
    }


    // turn on REUSEADDR, so it'll start right back up after dying
    {
        int flag = 1;
        int r;

        r = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*)&flag, sizeof(int));
        if (r < 0) printf("WARNING: ignoring setsockopt SO_REUSEADDR error: %s", strerror(errno));
    }



    // ok! listen for connections
    // we dont need to bind since listen on an unbound socket defaults to INADDR_ANY and a random port, which is what we want
    r = listen(sock, 20);
    if (r != 0) {
        printf("ERROR: cannot listen on port: %s\n", strerror(errno));
        goto fail0;
    }

    memset(&my_address, 0, sizeof(my_address));
    my_address_len = sizeof(my_address);
    r = getsockname(sock, (struct sockaddr *)&my_address, &my_address_len);
    if (r != 0) {
        fprintf(stderr, "bip: bip_init_publisher(): cannot get socket port: %s\n", strerror(errno));
        goto fail0;
    }



    // 
    // if we get here, the listening socket is set up and we're ready to start the publisher thread
    //


    // create the pipe for passing events back to the user
    r = pipe(bip_publisher_fds_to_user);
    if (r < 0) {
        printf("bip init_publisher: error making to-user pipe: %s\n", strerror(errno));
        goto fail0;
    }

    // create the pipe for getting subscription requests from the user
    r = pipe(bip_publisher_fds_from_user);
    if (r < 0) {
        printf("bip init_publisher: error making from-user pipe: %s\n", strerror(errno));
        goto fail1;
    }

    bip_publisher_thread = (pthread_t *)malloc(sizeof(pthread_t));
    if (bip_publisher_thread == NULL) {
        printf("bip init_publisher: cannot allocate memory for publisher thread: %s\n", strerror(errno));
        goto fail2;
    }


    // record the user's callback function
    cal_i.publisher_callback = callback;

    bip_listening_socket = sock;


    // start the publisher thread to talk to the peers
    r = pthread_create(bip_publisher_thread, NULL, bip_publisher_function, NULL);
    if (r != 0) {
        printf("bip_init_publisher(): cannot start publisher thread: %s\n", strerror(errno));
        goto fail3;
    }

    cal_peer_set_addressing_scheme(this, CAL_AS_IPv4);
    this->as.ipv4.socket = bip_publisher_fds_to_user[0];  // not really a socket...
    this->as.ipv4.port = ntohs(my_address.sin_port);

    return this->as.ipv4.socket;


fail3:
    free(bip_publisher_thread);
    bip_publisher_thread = NULL;

fail2:
    close(bip_publisher_fds_from_user[0]);
    close(bip_publisher_fds_from_user[1]);

fail1:
    close(bip_publisher_fds_to_user[0]);
    close(bip_publisher_fds_to_user[1]);

fail0:
    close(sock);
    return -1;
}


void bip_publish(char *topic, void *msg, int size) {
    printf("bip_publish is not implemented yet\n");
}


int bip_publisher_read(void) {
    int r;
    cal_event_t *event;

    r = read(bip_publisher_fds_to_user[0], &event, sizeof(cal_event_t*));
    if (r < 0) {
        printf("error in bip_read(): %s\n", strerror(errno));
        return 0;
    }
    if (r != sizeof(cal_event_t*)) {
        printf("bip_publisher_read(): short read of event pointer!\n");
        return 0;
    }

    if (cal_i.publisher_callback != NULL) {
        cal_i.publisher_callback(event);
    }

    cal_event_free(event);

    return 1;
}


int bip_publisher_sendto(cal_peer_t *peer, void *msg, int size) {
    if (peer->addressing_scheme != CAL_AS_IPv4) {
        fprintf(stderr, "bip_publisher_sendto(): peer %s (%s) has unknown addressing scheme, ignoring\n", peer->name, cal_peer_address_to_str(peer));
        return -1;
    }

    if (peer->as.ipv4.socket < 0) {
        fprintf(stderr, "bip_publisher_sendto(): peer %s (%s) has no socket, ignoring\n", peer->name, cal_peer_address_to_str(peer));
        return -1;
    }

    printf("sending \"%s\" (%d bytes) to %s\n", (char *)msg, size, peer->name);

    return write(peer->as.ipv4.socket, msg, size);
}

