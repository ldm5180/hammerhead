
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <errno.h>
#endif

#include <signal.h>

#include "cal-server.h"

extern cal_server_t cal_server;
static int cal_shutdown = 0;

void cal_callback(void * cal_handle, const cal_event_t *event) {
    if (cal_handle == NULL) {
	printf("CAL is not yet initialized.\n");
    }

    switch (event->type) {
        case CAL_EVENT_CONNECT: {
            printf("got a Connect event from %s\n", event->peer_name);
            break;
        }

        case CAL_EVENT_DISCONNECT: {
            printf("got a Disconnect event from %s\n", event->peer_name);
            break;
        }

        case CAL_EVENT_MESSAGE: {
            int i;
            char *msg = strdup("hey yourself");

            printf("got a Message event from %s, %d bytes:\n", event->peer_name, event->msg.size);
            for (i = 0; i < event->msg.size; i ++) {
                if ((i % 8) == 0) printf("    ");
                printf("%02X ", event->msg.buffer[i]);
                if ((i % 8) == 7) printf("\n");
            }
            if ((i % 8) != 7) printf("\n");

            cal_server.sendto(cal_handle, event->peer_name, msg, strlen(msg));

            break;
        }

        case CAL_EVENT_SUBSCRIBE: {
            printf("Client %s wants to subscribe to '%s'\n", event->peer_name, event->topic);
            cal_server.subscribe(cal_handle, event->peer_name, event->topic);
            break;
        }

        default: {
            printf("unhandled event type %d\n", event->type);
            break;
        }
    }
}




static void exit_signal_handler(int signal_number) {
    exit(0);
}


static void exit_handler(void) {
    cal_shutdown = 1;
}


void make_shutdowns_clean(void) {
    atexit(exit_handler);
#ifndef _WIN32
    struct sigaction sa;


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
#else
    signal (SIGINT, exit_signal_handler);
    signal (SIGTERM, exit_signal_handler);
#endif
}




int main(int argc, char *argv[]) {
    void * cal_handle;
    int r;
    int cal_fd;

    make_shutdowns_clean();


    cal_handle = cal_server.init("time", "time-publisher", cal_callback, NULL, NULL, 0);
    if (cal_handle == NULL) {
        printf("failed to init server\n");
        exit(1);
    }

    cal_fd = cal_server.get_fd(cal_handle);

    while (1) {
        fd_set readers;
        struct timeval timeout;

	if (cal_shutdown) {
	    cal_server.shutdown(cal_handle);
	}

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        FD_ZERO(&readers);
        FD_SET(cal_fd, &readers);

        r = select(cal_fd + 1, &readers, NULL, NULL, &timeout);
        if (r == -1) {
            printf("select fails: %s\n", strerror(errno));
            exit(1);
        }

        if (r == 0) {
            time_t t = time(NULL);
            char *s = ctime(&t);
            s[strlen(s)-1] = (char)0;
            cal_server.publish(cal_handle, "time", s, strlen(s)+1);
        }

        if (FD_ISSET(cal_fd, &readers)) {
            if (!cal_server.read(cal_handle, NULL, 1)) {
                printf("error reading cal event!\n");
                // FIXME
                exit(1);
            }
        }
    }

    // NOT REACHED

    exit(0);
}

