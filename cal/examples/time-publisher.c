
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cal.h"


#define Max(a, b) ((a) > (b) ? (a) : (b))


extern cal_pd_t cal_pd;
extern cal_i_t cal_i;  // FIXME: good god rename that


cal_peer_t me = {
    .name = "time-publisher",
    .addr.sa_family = AF_UNSPEC,
    .socket = -1,
    .user_data = NULL
};


void cal_i_callback(cal_event_t *event) {
    printf("cal_i callback!\n");
}


int main(int argc, char *argv[]) {
    while (1) {
        int cal_i_fd = -1;
        int max_fd;
        int r;
        fd_set readers;

        while (cal_i_fd < 0) {
            r = cal_pd.leave(&me);
            if (r < 0) {
                printf("failed to leave the peer list, oh well\n");
            }

            cal_i_fd = cal_i.init_publisher(&me, cal_i_callback);
            if (cal_i_fd < 0) {
                printf("failed to init publisher\n");
                sleep(1);
                continue;
            }

            r = cal_pd.join(&me);
            if (r < 0) {
                printf("failed to join the peer list\n");
                sleep(1);
            }
        }

        FD_ZERO(&readers);
        max_fd = -1;

        FD_SET(cal_i_fd, &readers);
        max_fd = Max(max_fd, cal_i_fd);

        printf("entering select\n");
        r = select(max_fd + 1, &readers, NULL, NULL, NULL);
        if (r == -1) {
            printf("select fails: %s\n", strerror(errno));
            exit(1);
        }

        if (FD_ISSET(cal_i_fd, &readers)) {
            if (!cal_i.publisher_read()) {
                printf("error reading cal-i event!\n");
                // FIXME
                exit(1);
            }
        }
    }

    // NOT REACHED

    exit(0);
}

