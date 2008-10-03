
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <sys/socket.h>

#include "cal.h"
#include "bip.h"


static pthread_t *cal_i_bip_subscriber_thread = NULL;

static int cal_i_bip_subscriber_fds_to_user[2];
static int cal_i_bip_subscriber_fds_from_user[2];


// this function is called by the subscriber thread when the user app wants to tell it something
void bip_subscriber_read_from_user(void) {
    bip_subscription_request_t *req;
    int r;

    r = read(cal_i_bip_subscriber_fds_from_user[0], &req, sizeof(req));
    if (r < 0) {
        printf("bip: error reading from user: %s\n", strerror(errno));
        return;
    } else if (r != sizeof(req)) {
        printf("bip: short read from user\n");
        return;
    }

    // FIXME: hope it's NULL terminated
    printf("read subscription request from user: peer=\"%s\", topic=\"%s\"\n", req->peer->name, req->topic);
}


//
// this function gets run by the thread started by init_subscriber()
// canceled by cancel_subscriber()
//

void *cal_i_bip_subscriber_function(void *arg) {
    // close(cal_i_bip_subscriber_fds_to_user[0]);
    // close(cal_i_bip_subscriber_fds_from_user[1]);

    while (1) {
        fd_set readers;
        int max_fd;
        int r;

        FD_ZERO(&readers);
        max_fd = -1;

        FD_SET(cal_i_bip_subscriber_fds_from_user[0], &readers);
        max_fd = Max(max_fd, cal_i_bip_subscriber_fds_from_user[0]);

        // block until there's something to do
        r = select(max_fd + 1, &readers, NULL, NULL, NULL);

        if (FD_ISSET(cal_i_bip_subscriber_fds_from_user[0], &readers)) {
            bip_subscriber_read_from_user();
        }
    }
}


int bip_init_subscriber(void (*callback)(cal_event_t *event)) {
    int r;

    // create the pipe for passing events back to the user
    r = pipe(cal_i_bip_subscriber_fds_to_user);
    if (r < 0) {
        printf("bip init_subscriber: error making to-user pipe: %s\n", strerror(errno));
        goto fail0;
    }

    // create the pipe for getting subscription requests from the user
    r = pipe(cal_i_bip_subscriber_fds_from_user);
    if (r < 0) {
        printf("bip init_subscriber: error making from-user pipe: %s\n", strerror(errno));
        goto fail1;
    }

    cal_i_bip_subscriber_thread = (pthread_t *)malloc(sizeof(pthread_t));
    if (cal_i_bip_subscriber_thread == NULL) {
        printf("bip init_subscriber: cannot allocate memory for subscriber thread: %s\n", strerror(errno));
        goto fail2;
    }

    // record the user's callback function
    cal_i.subscriber_callback = callback;

    // start the subscriber thread to talk to the peers
    r = pthread_create(cal_i_bip_subscriber_thread, NULL, cal_i_bip_subscriber_function, NULL);
    if (r != 0) {
        printf("dnssd subscribe_peer_list: cannot create browser thread: %s\n", strerror(errno));
        goto fail3;
    }

    // close(cal_i_bip_subscriber_fds_to_user[1]);
    // close(cal_i_bip_subscriber_fds_from_user[0]);

    // the from_user fd is written by bip.read(), the user doesnt need to know it

    return cal_i_bip_subscriber_fds_to_user[0];


fail3:
    free(cal_i_bip_subscriber_thread);
    cal_i_bip_subscriber_thread = NULL;
    cal_i.subscriber_callback = NULL;

fail2:
    close(cal_i_bip_subscriber_fds_from_user[0]);
    close(cal_i_bip_subscriber_fds_from_user[1]);

fail1:
    close(cal_i_bip_subscriber_fds_to_user[0]);
    close(cal_i_bip_subscriber_fds_to_user[1]);

fail0:
    return -1;
}




void bip_cancel_subscriber(void) {
    if (cal_i_bip_subscriber_thread != NULL) {
        int r;

        r = pthread_cancel(*cal_i_bip_subscriber_thread);
        if (r != 0) {
            printf("cal_i_bip_cancel_subscriber: error canceling browser thread: %s\n", strerror(errno));
            return;
        } else {
            pthread_join(*cal_i_bip_subscriber_thread, NULL);
            free(cal_i_bip_subscriber_thread);
            cal_i_bip_subscriber_thread = NULL;
        }
    }

    close(cal_i_bip_subscriber_fds_to_user[0]);
    close(cal_i_bip_subscriber_fds_to_user[1]);
    close(cal_i_bip_subscriber_fds_from_user[0]);
    close(cal_i_bip_subscriber_fds_from_user[1]);

    cal_i.subscriber_callback = NULL;
}




void bip_subscribe(cal_peer_t *peer, char *topic) {
    bip_subscription_request_t *req;
    int r;

    req = (bip_subscription_request_t *)malloc(sizeof(bip_subscription_request_t));
    if (req == NULL) {
        printf("out of memory!\n");
        return;
    } 

    // FIXME
    req->peer = peer;
    req->topic = strdup(topic);

    r = write(cal_i_bip_subscriber_fds_from_user[1], &req, sizeof(bip_subscription_request_t*));
    if (r < 0) {
        printf("bip_subscribe: error writing subscription request: %s\n", strerror(errno));
    } else if (r != sizeof(bip_subscription_request_t*)) {
        printf("bip_subscribe: short write of subscription request!\n");
    }
}




// this function is called by the user app when some published data has come in
int bip_subscriber_read(void) {
    int r;
    int i;
    char buffer[100];

    r = read(cal_i_bip_subscriber_fds_to_user[0], buffer, sizeof(buffer));
    if (r < 0) {
        printf("error in bip_subscriber_read(): %s\n", strerror(errno));
        return -1;
    }

    printf("bip_read got %d bytes:\n", r);
    for (i = 0; i < r; i ++) {
        printf("   %c\n", isprint(buffer[i]) ? buffer[i] : '.');
    }
    return r;
}

