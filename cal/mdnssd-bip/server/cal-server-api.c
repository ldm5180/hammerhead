
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

#include "cal-util.h"
#include "cal-server.h"
#include "cal-mdnssd-bip.h"
#include "cal-server-mdnssd-bip.h"




int cal_server_mdnssd_bip_init(cal_peer_t *this, void (*callback)(cal_event_t *event)) {
    int r;
    int sock;

    struct sockaddr_in my_address;
    socklen_t my_address_len;


    //
    // create the listening socket
    //

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        fprintf(stderr, ID "init: cannot create TCP socket: %s\n", strerror(errno));
        return -1;
    }


    // turn on REUSEADDR, so it'll start right back up after dying
    {
        int flag = 1;
        int r;

        r = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*)&flag, sizeof(int));
        if (r < 0) fprintf(stderr, ID "init: ignoring setsockopt SO_REUSEADDR error: %s", strerror(errno));
    }


    // ok! listen for connections
    // we dont need to bind since listen on an unbound socket defaults to INADDR_ANY and a random port, which is what we want
    r = listen(sock, 20);
    if (r != 0) {
        fprintf(stderr, ID "init: cannot listen on port: %s\n", strerror(errno));
        goto fail0;
    }

    memset(&my_address, 0, sizeof(my_address));
    my_address_len = sizeof(my_address);
    r = getsockname(sock, (struct sockaddr *)&my_address, &my_address_len);
    if (r != 0) {
        fprintf(stderr, ID "init: cannot get socket port: %s\n", strerror(errno));
        goto fail0;
    }


    // 
    // if we get here, the listening socket is set up and we're ready to start the publisher thread
    //


    // create the pipe for passing events back to the user
    r = pipe(cal_server_mdnssd_bip_fds_to_user);
    if (r < 0) {
        fprintf(stderr, ID "init: error making to-user pipe: %s\n", strerror(errno));
        goto fail0;
    }

    // create the pipe for getting subscription requests from the user
    r = pipe(cal_server_mdnssd_bip_fds_from_user);
    if (r < 0) {
        fprintf(stderr, ID "init: error making from-user pipe: %s\n", strerror(errno));
        goto fail1;
    }

    cal_server_mdnssd_bip_thread = (pthread_t *)malloc(sizeof(pthread_t));
    if (cal_server_mdnssd_bip_thread == NULL) {
        fprintf(stderr, ID "init: cannot allocate memory for publisher thread: %s\n", strerror(errno));
        goto fail2;
    }

    // record the user's callback function
    cal_server.callback = callback;

    cal_server_mdnssd_bip_listening_socket = sock;


    // start the publisher thread to talk to the peers
    r = pthread_create(cal_server_mdnssd_bip_thread, NULL, cal_server_mdnssd_bip_function, this);
    if (r != 0) {
        fprintf(stderr, ID "init: cannot start publisher thread: %s\n", strerror(errno));
        goto fail3;
    }

    cal_peer_set_addressing_scheme(this, CAL_AS_IPv4);
    this->as.ipv4.socket = cal_server_mdnssd_bip_fds_to_user[0];  // not really a socket...
    this->as.ipv4.port = ntohs(my_address.sin_port);

    return this->as.ipv4.socket;


fail3:
    free(cal_server_mdnssd_bip_thread);
    cal_server_mdnssd_bip_thread = NULL;

fail2:
    close(cal_server_mdnssd_bip_fds_from_user[0]);
    close(cal_server_mdnssd_bip_fds_from_user[1]);

fail1:
    close(cal_server_mdnssd_bip_fds_to_user[0]);
    close(cal_server_mdnssd_bip_fds_to_user[1]);

fail0:
    close(sock);
    return -1;
}




void cal_server_mdnssd_bip_shutdown(void) {
    if (cal_server_mdnssd_bip_thread != NULL) {
        int r;

        r = pthread_cancel(*cal_server_mdnssd_bip_thread);
        if (r != 0) {
            fprintf(stderr, ID "shutdown: error canceling server thread: %s\n", strerror(errno));
            return;
        } else {
            pthread_join(*cal_server_mdnssd_bip_thread, NULL);
            free(cal_server_mdnssd_bip_thread);
            cal_server_mdnssd_bip_thread = NULL;
        }
    }

    close(cal_server_mdnssd_bip_fds_to_user[0]);
    close(cal_server_mdnssd_bip_fds_to_user[1]);

    close(cal_server_mdnssd_bip_fds_from_user[0]);
    close(cal_server_mdnssd_bip_fds_from_user[1]);

    cal_server.callback = NULL;
}




int cal_server_mdnssd_bip_read(void) {
    int r;
    cal_event_t *event;

    r = read(cal_server_mdnssd_bip_fds_to_user[0], &event, sizeof(cal_event_t*));
    if (r < 0) {
        fprintf(stderr, ID "read: error: %s\n", strerror(errno));
        return 0;
    } else if (r != sizeof(cal_event_t*)) {
        fprintf(stderr, ID "read: short read of event pointer!\n");
        return 0;
    } else if (event == NULL) {
        fprintf(stderr, ID "read: got NULL event!\n");
        return 0;
    }

    if (cal_server.callback != NULL) {
        cal_server.callback(event);
    }

    // manage memory
    // FIXME
    switch (event->type) {
        case CAL_EVENT_CONNECT: {
            event->peer = NULL;  // the peer is still around until the Disconnect
            break;
        }

        case CAL_EVENT_MESSAGE: {
            event->peer = NULL;  // the peer is still around until the Disconnect
            break;
        }
        case CAL_EVENT_DISCONNECT: {
            break;
        }

        default: {
            fprintf(stderr, ID "read: got unhandled event type %d\n", event->type);
            break;
        }
    }

    cal_event_free(event);

    return 1;
}




// FIXME: prolly pass a Message event to the cal thread
int cal_server_mdnssd_bip_sendto(cal_peer_t *peer, void *msg, int size) {
    if (peer->addressing_scheme != CAL_AS_IPv4) {
        fprintf(stderr, ID "sendto(): peer %s (%s) has unknown addressing scheme, ignoring\n", peer->name, cal_peer_address_to_str(peer));
        return -1;
    }

    if (peer->as.ipv4.socket < 0) {
        fprintf(stderr, ID "sendto(): peer %s (%s) has no socket, ignoring\n", peer->name, cal_peer_address_to_str(peer));
        return -1;
    }

    printf("sending \"%s\" (%d bytes) to %s\n", (char *)msg, size, peer->name);

    return write(peer->as.ipv4.socket, msg, size);
}




void cal_server_mdnssd_bip_publish(char *topic, void *msg, int size) {
    printf(ID "publish: not implemented yet\n");
}




cal_server_t cal_server = {
    .callback = NULL,

    .init = cal_server_mdnssd_bip_init,
    .shutdown = cal_server_mdnssd_bip_shutdown,

    .read = cal_server_mdnssd_bip_read,
    .sendto = cal_server_mdnssd_bip_sendto,
    .publish = cal_server_mdnssd_bip_publish
};

