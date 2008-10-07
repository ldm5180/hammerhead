
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <sys/socket.h>

#include "cal-client.h"




extern cal_client_t cal_client;




void cal_callback(cal_event_t *event) {
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
            cal_client.sendto(event->peer, "hey there!", 11);
            cal_client.subscribe(event->peer, "time");

            // steal this peer from the event
            event->peer = NULL;

            break;
        }

        case CAL_EVENT_LEAVE: {
            printf("Leave event from '%s'\n", event->peer->name);
            break;
        }

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
    int cal_fd = -1;

    while (1) {
        int r;
        fd_set readers;


        while (cal_fd == -1) {
            cal_fd = cal_client.init(cal_callback);
            if (cal_fd < 0) {
                sleep(1);
                continue;
            }
        }


	FD_ZERO(&readers);
	FD_SET(cal_fd, &readers);

	r = select(cal_fd + 1, &readers, NULL, NULL, NULL);
	if (r == -1) {
	    printf("select fails: %s\n", strerror(errno));
            cal_client.shutdown();
            cal_fd = -1;
	    continue;
	}

        if (FD_ISSET(cal_fd, &readers)) {
            if (!cal_client.read()) {
                printf("error reading CAL event!\n");
                cal_client.shutdown();
                cal_fd = -1;
                continue;
            }
        }
    }

    // NOT REACHED

    exit(0);
}

