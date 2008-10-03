
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cal.h"


#define Max(a, b) ((a) > (b) ? (a) : (b))


extern cal_pd_t cal_pd;
extern cal_i_t cal_i;  // FIXME: good god rename that


cal_peer_t *me;


void cal_i_callback(cal_event_t *event) {
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

            printf("got a Message event from %s (%s), %d bytes:\n", event->peer->name, cal_peer_address_to_str(event->peer), event->msg.size);
            for (i = 0; i < event->msg.size; i ++) {
                if ((i % 8) == 0) printf("    ");
                printf("%02X ", event->msg.buffer[i]);
                if ((i % 8) == 7) printf("\n");
            }
            if ((i % 8) != 7) printf("\n");

            event->peer = NULL;  // FIXME: cal-i requires that you do this

            break;
        }

        default: {
            printf("unhandled event type %d\n", event->type);
            break;
        }
    }
}


int main(int argc, char *argv[]) {
    int cal_i_fd;
    int r;


    me = cal_peer_new("time-publisher");

    cal_i_fd = cal_i.init_publisher(me, cal_i_callback);
    if (cal_i_fd < 0) {
        printf("failed to init publisher\n");
        exit(1);
    }

    r = cal_pd.join(me);
    if (r < 0) {
        printf("failed to join the peer list\n");
        exit(1);
    }


    while (1) {
        int max_fd;
        fd_set readers;

        FD_ZERO(&readers);
        max_fd = -1;

        FD_SET(cal_i_fd, &readers);
        max_fd = Max(max_fd, cal_i_fd);

        printf("entering select\n");
        r = select(max_fd + 1, &readers, NULL, NULL, NULL);
        if (r == -1) {
            printf("select fails: %s\n", strerror(errno));
            exit(1);
        }

        if (FD_ISSET(cal_i_fd, &readers)) {
            if (!cal_i.publisher_read()) {
                printf("error reading cal-i event!\n");
                // FIXME
                exit(1);
            }
        }
    }

    // NOT REACHED

    exit(0);
}

