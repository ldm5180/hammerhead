
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cal-server.h"


cal_peer_t *this;


void cal_callback(cal_event_t *event) {
    switch (event->type) {
        case CAL_EVENT_CONNECT: {
            printf("got a Connect event from %s (%s)\n", event->peer->name, cal_peer_address_to_str(event->peer));
            event->peer = NULL;  // FIXME: cal-i requires that you do this
            break;
        }

        case CAL_EVENT_DISCONNECT: {
            printf("got a Disconnect event from %s (%s)\n", event->peer->name, cal_peer_address_to_str(event->peer));
            break;
        }

        case CAL_EVENT_MESSAGE: {
            int i;
            char *msg = "hey yourself";

            printf("got a Message event from %s (%s), %d bytes:\n", event->peer->name, cal_peer_address_to_str(event->peer), event->msg.size);
            for (i = 0; i < event->msg.size; i ++) {
                if ((i % 8) == 0) printf("    ");
                printf("%02X ", event->msg.buffer[i]);
                if ((i % 8) == 7) printf("\n");
            }
            if ((i % 8) != 7) printf("\n");

            cal_server.sendto(event->peer, msg, strlen(msg));

            break;
        }

        default: {
            printf("unhandled event type %d\n", event->type);
            break;
        }
    }
}


int main(int argc, char *argv[]) {
    int cal_fd;
    int r;


    this = cal_peer_new("time-publisher");

    cal_fd = cal_server.init(this, cal_callback);
    if (cal_fd < 0) {
        printf("failed to init server\n");
        exit(1);
    }


    while (1) {
        fd_set readers;

        FD_ZERO(&readers);
        FD_SET(cal_fd, &readers);

        printf("entering select\n");
        r = select(cal_fd + 1, &readers, NULL, NULL, NULL);
        if (r == -1) {
            printf("select fails: %s\n", strerror(errno));
            exit(1);
        }

        if (FD_ISSET(cal_fd, &readers)) {
            if (!cal_server.read()) {
                printf("error reading cal event!\n");
                // FIXME
                exit(1);
            }
        }
    }

    // NOT REACHED

    exit(0);
}

