
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <netinet/in.h>

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




static void exit_signal_handler(int signal_number) {
    exit(0);
}


static void exit_handler(void) {
    cal_client.shutdown();
}


void make_shutdowns_clean(void) {
    struct sigaction sa;

    atexit(exit_handler);

    // handle exit signals
    sa.sa_handler = exit_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGINT, &sa, NULL) < 0) {
        fprintf(stderr, "sigaction(SIGINT, ...): %s", strerror(errno));
        exit(1);
    }

    if (sigaction(SIGTERM, &sa, NULL) < 0) {
        fprintf(stderr, "sigaction(SIGTERM, ...): %s", strerror(errno));
        exit(1);
    }


    //
    // If a network connection is lost somehow, writes will cause the
    // process to receive SIGPIPE, and the default SIGPIPE handler
    // terminates the process.  Not so good.  So we change the handler to
    // ignore the signal, and we explicitly check for the error when we
    // need to.
    //

    sa.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &sa, NULL) < 0) {
        fprintf(stderr, "error setting SIGPIPE sigaction to SIG_IGN: %s", strerror(errno));
        exit(1);
    }
}




int main(int argc, char *argv[]) {
    int cal_fd = -1;


    make_shutdowns_clean();


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

