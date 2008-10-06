
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <sys/socket.h>

#include "cal.h"


#define Max(a, b) ((a) > (b) ? (a) : (b))


extern cal_pd_t cal_pd;
extern cal_i_t cal_i;  // FIXME: good god rename that


void cal_pd_callback(cal_event_t *event) {
    switch (event->type) {
        case CAL_EVENT_JOIN: {
            printf(
                "Join event from '%s' (%s)\n",
                event->peer->name,
                cal_peer_address_to_str(event->peer)
            );

            if (strcmp(event->peer->name, "time-publisher") != 0) {
                break;
            }

            printf("found a time-publisher, subscribing to the time from it\n");
            cal_i.subscriber_sendto(event->peer, "hey there!", 11);
            cal_i.subscribe(event->peer, "time");

            // steal this peer from the event
            event->peer = NULL;

            break;
        }

        case CAL_EVENT_LEAVE: {
            printf("Leave event from '%s'\n", event->peer->name);
            break;
        }

        default: {
            printf("unknown event type %d\n", event->type);
            break;
        }
    }

    cal_event_free(event);
}


void cal_i_callback(cal_event_t *event) {
    switch (event->type) {
        case CAL_EVENT_MESSAGE: {
            int i;

            printf(
                "Message event from '%s' (%s)\n",
                event->peer->name,
                cal_peer_address_to_str(event->peer)
            );

            for (i = 0; i < event->msg.size; i ++) {
                if ((i % 8) == 0) printf("    ");
                printf("%02X ", event->msg.buffer[i]);
                if ((i % 8) == 7) printf("\n");
            }
            if ((i % 8) != 7) printf("\n");

            break;
        }

        default: {
            printf("unknown event type %d\n", event->type);
            break;
        }
    }
}


int main(int argc, char *argv[]) {
    int cal_pd_fd = -1;
    int cal_i_fd = -1;

    while (1) {
        int max_fd;
        int r;
        fd_set readers;


        while (cal_pd_fd == -1) {
            cal_pd_fd = cal_pd.subscribe_peer_list(cal_pd_callback);
            if (cal_pd_fd < 0) {
                sleep(1);
                continue;
            }
        }


        while (cal_i_fd == -1) {
            cal_i_fd = cal_i.init_subscriber(cal_i_callback);
            if (cal_i_fd < 0) {
                sleep(1);
                continue;
            }
        }


	FD_ZERO(&readers);
        max_fd = -1;

	FD_SET(cal_pd_fd, &readers);
        max_fd = Max(cal_pd_fd, max_fd);

	FD_SET(cal_i_fd, &readers);
        max_fd = Max(cal_i_fd, max_fd);

	r = select(max_fd + 1, &readers, NULL, NULL, NULL);
	if (r == -1) {
	    printf("select fails: %s\n", strerror(errno));

            cal_pd.unsubscribe_peer_list();
            cal_pd_fd = -1;

            cal_i.cancel_subscriber();
            cal_i_fd = -1;

	    continue;
	}

        if (FD_ISSET(cal_pd_fd, &readers)) {
            if (!cal_pd.read()) {
                printf("error reading CAL-PD event!\n");
                cal_pd.unsubscribe_peer_list();
                cal_pd_fd = -1;
                continue;
            }
        }

        if (FD_ISSET(cal_i_fd, &readers)) {
            if (!cal_i.subscriber_read()) {
                printf("error reading CAL-I event!\n");
                cal_i.cancel_subscriber();
                cal_i_fd = -1;
                continue;
            }
        }

    }

    // NOT REACHED

    exit(0);
}

