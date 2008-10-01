
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
    struct sockaddr_in *sin;


    memset(&me, '\0', sizeof(me));
    me.name = strdup("a random cal-test program");
    me.socket = -1;

    me.addr.sa_family = AF_INET;
    sin = (struct sockaddr_in *)&me.addr;
    sin->sin_port = htons(12345);           // a fake port, good as any
    sin->sin_addr.s_addr = INADDR_ANY;      // this part is ignored for now, cal-pd/mdns-sd advertises on all available interfaces

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

