
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "glib-on-cal.h"


bionet_hab_t me;


static void exit_signal_handler(int signal_number) {
    printf("caught signal %d\n", signal_number);
    exit(0);
}


static void exit_handler(void) {
    printf("exiting\n");
    leave(&me);
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
    make_shutdowns_clean();

    me.name = "Some-HAB-Type.Some-HAB-ID";
    join(&me);

    while (1) {
        pause();
    }

    // NOT REACHED
    return 0;
}

