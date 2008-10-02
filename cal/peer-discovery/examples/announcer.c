
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "cal.h"


extern cal_pd_t cal_pd;


cal_peer_t me;


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


    memset(&me, '\0', sizeof(me));
    me.name = strdup("a random cal-test program");

    cal_peer_set_addressing_scheme(&me, CAL_AS_IPv4);
    me.as.ipv4.port = 12345;   // a fake port, good as any

    if (argc > 1) {
        me.name = argv[1];
        printf("using '%s' for the peer name\n", me.name);
    }


    make_shutdowns_clean();


    r = cal_pd.join(&me);
    if (!r) {
        printf("error joining!\n");
        return 1;
    }
    printf("joined!\n");

    while (1) {
        pause();
    }

    // NOT REACHED
    return 1;
}

