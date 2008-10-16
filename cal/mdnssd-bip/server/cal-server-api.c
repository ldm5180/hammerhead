
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




int cal_server_mdnssd_bip_init(cal_peer_t *this, void (*callback)(const cal_event_t *event)) {
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

    cal_peer_set_addressing_scheme(this, CAL_AS_IPv4);
    this->as.ipv4.socket = cal_server_mdnssd_bip_fds_to_user[0];  // not really a socket...
    this->as.ipv4.port = ntohs(my_address.sin_port);


    // start the publisher thread to talk to the peers
    r = pthread_create(cal_server_mdnssd_bip_thread, NULL, cal_server_mdnssd_bip_function, this);
    if (r != 0) {
        fprintf(stderr, ID "init: cannot start publisher thread: %s\n", strerror(errno));
        goto fail3;
    }


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
    switch (event->type) {
        case CAL_EVENT_CONNECT: {
            event->peer = NULL;  // the peer is still around until the Disconnect
            break;
        }

        case CAL_EVENT_MESSAGE: {
            event->peer = NULL;  // the peer is still around until the Disconnect
            break;
        }

        case CAL_EVENT_SUBSCRIBE: {
            event->peer = NULL;  // the peer is still around until the Disconnect
            break;
        }

        case CAL_EVENT_DISCONNECT: {
            break;
        }

        default: {
            fprintf(stderr, ID "read: got unhandled event type %d\n", event->type);
            return 1;  // dont free events we dont understand
        }
    }

    cal_event_free(event);

    return 1;
}




int cal_server_mdnssd_bip_sendto(const cal_peer_t *peer, void *msg, int size) {
    int r;
    cal_event_t *event;

    event = cal_event_new(CAL_EVENT_MESSAGE);
    if (event == NULL) {
        return 0;
    }

    event->peer = (cal_peer_t *)peer;
    event->msg.buffer = msg;
    event->msg.size = size;

    r = write(cal_server_mdnssd_bip_fds_from_user[1], &event, sizeof(event));
    if (r < 0) {
        fprintf(stderr, ID "sendto(): error writing to server thread: %s", strerror(errno));
        return 0;
    }
    if (r < sizeof(event)) {
        fprintf(stderr, ID "sendto(): short write to server thread!!");
        return 0;
    }

    return 1;
}




void cal_server_mdnssd_bip_publish(const char *topic, const void *msg, int size) {
    int r;
    cal_event_t *event;

    event = cal_event_new(CAL_EVENT_PUBLISH);
    if (event == NULL) {
        fprintf(stderr, ID "publish: out of memory\n");
        return;
    }

    event->topic = strdup(topic);
    if (event->topic == NULL) {
        fprintf(stderr, ID "publish: out of memory\n");
        return;
    }

    event->msg.buffer = malloc(size);
    if (event->msg.buffer == NULL) {
        fprintf(stderr, ID "publish: out of memory\n");
        return;
    }
    memcpy(event->msg.buffer, msg, size);
    event->msg.size = size;

    r = write(cal_server_mdnssd_bip_fds_from_user[1], &event, sizeof(event));
    if (r < 0) {
        fprintf(stderr, ID "publish: error writing to server thread: %s", strerror(errno));
        return;
    }
    if (r < sizeof(event)) {
        fprintf(stderr, ID "publish: short write to server thread!!");
        return;
    }
}




cal_server_t cal_server = {
    .callback = NULL,

    .init = cal_server_mdnssd_bip_init,
    .shutdown = cal_server_mdnssd_bip_shutdown,

    .read = cal_server_mdnssd_bip_read,
    .sendto = cal_server_mdnssd_bip_sendto,
    .publish = cal_server_mdnssd_bip_publish
};

