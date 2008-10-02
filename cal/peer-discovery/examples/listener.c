
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "cal.h"


extern cal_pd_t cal_pd;


static void exit_signal_handler(int signal_number) {
    printf("caught signal %d\n", signal_number);
    exit(0);
}


static void exit_handler(void) {
    printf("exiting\n");
    cal_pd.unsubscribe_peer_list();
}


static void make_shutdowns_clean(void) {
    struct sigaction exit_sigaction;

    atexit(exit_handler);

    // handle signals
    exit_sigaction.sa_handler = exit_signal_handler;
    sigemptyset(&exit_sigaction.sa_mask);
    exit_sigaction.sa_flags = SA_RESTART;

    if (sigaction(SIGINT, &exit_sigaction, NULL) < 0) {
        printf("sigaction(SIGINT, ...): %s\n", strerror(errno));
        exit(1);
    }

    if (sigaction(SIGTERM, &exit_sigaction, NULL) < 0) {
        printf("sigaction(SIGTERM, ...): %s\n", strerror(errno));
        exit(1);
    }
}




void callback(cal_event_t *event) {
    switch (event->event_type) {
        case CAL_EVENT_JOIN: {
            printf(
                "Join event from '%s' (%s)\n",
                event->peer->name,
                cal_peer_address_to_str(event->peer)
            );
            break;
        }

        case CAL_EVENT_LEAVE: {
            printf("Leave event from '%s'\n", event->peer->name);
            break;
        }

        default: {
            printf("unknown event type %d\n", event->event_type);
            break;
        }
    }

    cal_event_free(event);
}




int main(int argc, char *argv[]) {
    int cal_event_fd = -1;

    make_shutdowns_clean();

    while (1) {
        int r;
        fd_set readers;


        while (cal_event_fd == -1) {
            cal_event_fd = cal_pd.subscribe_peer_list(callback);
            if (cal_event_fd < 0) {
                sleep(1);
                continue;
            }
        }


	FD_ZERO(&readers);
	FD_SET(cal_event_fd, &readers);

        // printf("entering select\n");
	r = select(cal_event_fd + 1, &readers, NULL, NULL, NULL);
	if (r == -1) {
	    printf("select fails: %s\n", strerror(errno));
            cal_pd.unsubscribe_peer_list();
            cal_event_fd = -1;
	    continue;
	}

        if (!cal_pd.read()) {
            printf("error reading event!\n");
            cal_pd.unsubscribe_peer_list();
            cal_event_fd = -1;
            continue;
        }
    }

    // NOT REACHED

    exit(0);
}

