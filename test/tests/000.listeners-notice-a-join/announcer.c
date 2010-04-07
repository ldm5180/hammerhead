
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "cal.h"


extern cal_pd_t cal_pd;


const char *unicast_address[] = {
    "bip://localhost:666"
};

cal_peer_t me = {
    .name = "a random cal-test program",
    .num_unicast_addresses = 1,
    .unicast_address = (char **)unicast_address,
    .user_data = NULL
};


static void exit_signal_handler(int signal_number) {
    printf("caught signal %d\n", signal_number);
    exit(0);
}


static void exit_handler(void) {
    printf("exiting\n");
    cal_pd.leave(&me);
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


int main(int argc, char *argv[]) {
    int r;
    int timeout = 0;
    time_t start, now;


    {
        int i;

        for (i = 1; i < argc; i ++) {
            if (strcmp(argv[i], "--peer") == 0) {
                i ++;
                me.name = argv[i];

            } else if (strcmp(argv[i], "--timeout") == 0) {
                i ++;
                timeout = atoi(argv[i]);

            } else {
                printf("unknown command-line argument '%s'\n", argv[i]);
                exit(2);
            }
        }
    }


    printf("starting announcer\n");


    start = time(NULL);


    make_shutdowns_clean();


    r = cal_pd.join(&me);
    if (!r) {
        printf("error joining!\n");
        return 1;
    }
    printf("joined!\n");

    while (1) {
        if (timeout > 0) {
            int time_to_wait;
            now = time(NULL);
            time_to_wait = timeout - (now-start);
            if (time_to_wait <= 0) break;
            sleep(time_to_wait);
        } else {
            pause();
        }
    }

    exit(0);
}

