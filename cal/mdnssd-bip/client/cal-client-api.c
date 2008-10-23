
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <glib.h>

#include "cal-client.h"
#include "cal-client-mdnssd-bip.h"




int cal_client_mdnssd_bip_init(void (*callback)(const cal_event_t *event)) {
    int r;


    // create the pipe for passing events back to the user thread
    r = pipe(cal_client_mdnssd_bip_fds_to_user);
    if (r < 0) {
        printf(ID "init(): error making to-user pipe: %s\n", strerror(errno));
        goto fail0;
    }

    // create the pipe for getting subscription requests from the user
    r = pipe(cal_client_mdnssd_bip_fds_from_user);
    if (r < 0) {
        printf(ID "init(): error making from-user pipe: %s\n", strerror(errno));
        goto fail1;
    }

    cal_client_mdnssd_bip_thread = (pthread_t *)malloc(sizeof(pthread_t));
    if (cal_client_mdnssd_bip_thread == NULL) {
        printf(ID "init(): cannot allocate memory for thread: %s\n", strerror(errno));
        goto fail2;
    }

    // record the user's callback function
    cal_client.callback = callback;

    // start the Client thread
    r = pthread_create(cal_client_mdnssd_bip_thread, NULL, cal_client_mdnssd_bip_function, NULL);
    if (r != 0) {
        printf(ID "init(): cannot create thread: %s\n", strerror(errno));
        goto fail3;
    }


    return cal_client_mdnssd_bip_fds_to_user[0];


fail3:
    free(cal_client_mdnssd_bip_thread);
    cal_client_mdnssd_bip_thread = NULL;
    cal_client.callback = NULL;

fail2:
    close(cal_client_mdnssd_bip_fds_from_user[0]);
    close(cal_client_mdnssd_bip_fds_from_user[1]);

fail1:
    close(cal_client_mdnssd_bip_fds_to_user[0]);
    close(cal_client_mdnssd_bip_fds_to_user[1]);

fail0:
    return -1;
}




void cal_client_mdnssd_bip_shutdown(void) {
    if (cal_client_mdnssd_bip_thread != NULL) {
        int r;

        r = pthread_cancel(*cal_client_mdnssd_bip_thread);
        if (r != 0) {
            fprintf(stderr, ID "shutdown(): error canceling client thread: %s\n", strerror(errno));
            return;
        } else {
            pthread_join(*cal_client_mdnssd_bip_thread, NULL);
            free(cal_client_mdnssd_bip_thread);
            cal_client_mdnssd_bip_thread = NULL;
        }
    }

    close(cal_client_mdnssd_bip_fds_to_user[0]);
    close(cal_client_mdnssd_bip_fds_to_user[1]);
    close(cal_client_mdnssd_bip_fds_from_user[0]);
    close(cal_client_mdnssd_bip_fds_from_user[1]);

    cal_client.callback = NULL;
}




int cal_client_mdnssd_bip_subscribe(const char *peer_name, const char *topic) {
    int r;
    cal_event_t *event;

    event = cal_event_new(CAL_EVENT_SUBSCRIBE);
    if (event == NULL) {
        return 0;
    }

    event->topic = strdup(topic);
    if (event->topic == NULL) {
        fprintf(stderr, ID "subscribe: out of memory\n");
        return 0;
    }

    event->peer_name = strdup(peer_name);
    if (event->peer_name == NULL) {
        fprintf(stderr, ID "subscribe: out of memory\n");
        cal_event_free(event);
        return 0;
    }

    r = write(cal_client_mdnssd_bip_fds_from_user[1], &event, sizeof(event));
    if (r < 0) {
        fprintf(stderr, ID "subscribe(): error writing to client thread: %s", strerror(errno));
        return 0;
    }
    if (r < sizeof(event)) {
        fprintf(stderr, ID "subscribe(): short write to client thread!!");
        return 0;
    }

    return 1;
}




int cal_client_mdnssd_bip_read(void) {
    cal_event_t *event;
    int r;

    r = read(cal_client_mdnssd_bip_fds_to_user[0], &event, sizeof(event));
    if (r < 0) {
        fprintf(stderr, ID "read(): error: %s\n", strerror(errno));
        return 0;
    } else if (r != sizeof(event)) {
        fprintf(stderr, ID "read(): short read from client thread\n");
        return 0;
    } else if (event == NULL) {
        fprintf(stderr, ID "read(): got NULL event!\n");
        return 0;
    }

    if (cal_client.callback != NULL) {
        cal_client.callback(event);
    }

    // manage memory
    switch (event->type) {
        case CAL_EVENT_JOIN: {
            event->peer = NULL;  // the CAL Client thread has a copy, we'll free it when the peer leaves later
            break;
        }

        case CAL_EVENT_LEAVE: {
            break;
        }

        case CAL_EVENT_MESSAGE: {
            event->peer = NULL;  // the CAL Client thread has a copy, we'll free it when the peer leaves later
            break;
        }

        case CAL_EVENT_PUBLISH: {
            event->peer = NULL;  // the CAL Client thread has a copy, we'll free it when the peer leaves later
            break;
        }

        default: {
            fprintf(stderr, ID "read(): got unhandled event type %d\n", event->type);
            return 1;  // dont free events we dont understand
        }
    }

    cal_event_free(event);

    return 1;
}




int cal_client_mdnssd_bip_sendto(const char *peer_name, void *msg, int size) {
    int r;
    cal_event_t *event;

    event = cal_event_new(CAL_EVENT_MESSAGE);
    if (event == NULL) {
        fprintf(stderr, ID "sendto: out of memory");
        return 0;
    }

    event->peer_name = strdup(peer_name);
    if (event->peer_name == NULL) {
        cal_event_free(event);
        fprintf(stderr, ID "sendto: out of memory");
        return 0;
    }

    event->msg.buffer = msg;
    event->msg.size = size;

    r = write(cal_client_mdnssd_bip_fds_from_user[1], &event, sizeof(event));
    if (r < 0) {
        fprintf(stderr, ID "sendto(): error writing to client thread: %s", strerror(errno));
        return 0;
    }
    if (r < sizeof(event)) {
        fprintf(stderr, ID "sendto(): short write to client thread!!");
        return 0;
    }

    return 1;
}




cal_client_t cal_client = {
    .callback = NULL,

    .init = cal_client_mdnssd_bip_init,
    .shutdown = cal_client_mdnssd_bip_shutdown,

    .subscribe = cal_client_mdnssd_bip_subscribe,

    .read = cal_client_mdnssd_bip_read,

    .sendto = cal_client_mdnssd_bip_sendto
};

