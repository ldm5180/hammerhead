
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#include "cal.h"


extern cal_pd_t cal_pd;

char *expected_peer_name = NULL;


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

    printf("registering exit handler...\n");

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
            if (expected_peer_name != NULL) {
                if (strcmp(expected_peer_name, event->peer.name) != 0) {
                    printf("got Join from unexpected peer '%s', ignoring\n", event->peer.name);
                    break;
                }
            }
            // test passes!
            printf("Join event from expected peer '%s'\n", event->peer.name);
            printf("success!\n");
            exit(0);
        }

        case CAL_EVENT_LEAVE: {
            printf("Leave event from '%s'\n", event->peer.name);
            break;
        }

        default: {
            printf("unknown event type %d\n", event->event_type);
            break;
        }
    }

    cal_pd_free_event(event);
}




int main(int argc, char *argv[]) {
    int cal_event_fd = -1;
    int timeout = -1;

    time_t start;


    {
        int i;

        for (i = 1; i < argc; i ++) {
            if (strcmp(argv[i], "--timeout") == 0) {
                i ++;
                timeout = atoi(argv[i]);
            }
            
            else if (strcmp(argv[i], "--peer") == 0) {
                i ++;
                expected_peer_name = argv[i];

            } else {
                printf("unknown command-line argument: %s\n", argv[i]);
                exit(2);
            }
        }
    }


    printf("starting listener\n");
    printf("timeout is %d seconds\n", timeout);


    make_shutdowns_clean();

    start = time(NULL);
    if (start == -1) {
        printf("error with time: %s\n", strerror(errno));
        exit(2);
    }


    while (1) {
        int r;
        fd_set readers;
        time_t now;
        struct timeval this_timeout;
        struct timeval *use_timeout;


        while (cal_event_fd == -1) {
            cal_event_fd = cal_pd.subscribe_peer_list(callback);
            if (cal_event_fd < 0) {
                printf("error subscribing to peer list!\n");
                exit(1);
            }
        }


	FD_ZERO(&readers);
	FD_SET(cal_event_fd, &readers);

        now = time(NULL);
        if (now == -1) {
            printf("error with time: %s\n", strerror(errno));
            exit(2);
        }

        if (timeout > 0) {
            this_timeout.tv_sec = timeout - (now - start);
            this_timeout.tv_usec = 0;
            if (this_timeout.tv_sec <= 0) {
                printf("timeout!  no joy!\n");
                exit(1);
            }
            use_timeout = &this_timeout;
        } else {
            use_timeout = NULL;
        }

        printf("entering select\n");
	r = select(cal_event_fd + 1, &readers, NULL, NULL, use_timeout);
	if (r == -1) {
	    printf("select fails: %s\n", strerror(errno));
            exit(2);
	}

        if (r == 0) {
            // timeout
            continue;
        }

        if (!cal_pd.read()) {
            printf("error reading event!\n");
            exit(1);
        }
    }

    // NOT REACHED

    exit(0);
}

