
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
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




int cal_client_mdnssd_bip_init(
    void (*callback)(const cal_event_t *event),
    int (*peer_matches)(const char *peer_name, const char *subscription)
) {
    int r;

    cal_client_mdnssd_bip_t *this;


    // set up the context
    this = (cal_client_mdnssd_bip_t *)calloc(1, sizeof(cal_client_mdnssd_bip_t));
    if (this == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "init: out of memory!");
        goto fail0;
    }

    if (peer_matches == NULL) {
        this->peer_matches = strcmp;
    } else {
        this->peer_matches = peer_matches;
    }

    // create the pipe for passing events back to the user thread
    r = pipe(cal_client_mdnssd_bip_fds_to_user);
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: error making to-user pipe: %s", strerror(errno));
        goto fail1;
    }

    // create the pipe for getting subscription requests from the user
    r = pipe(cal_client_mdnssd_bip_fds_from_user);
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: error making from-user pipe: %s", strerror(errno));
        goto fail2;
    }

    cal_client_mdnssd_bip_thread = (pthread_t *)malloc(sizeof(pthread_t));
    if (cal_client_mdnssd_bip_thread == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: cannot allocate memory for thread: %s", strerror(errno));
        goto fail3;
    }

    // record the user's callback function
    cal_client.callback = callback;

    // start the Client thread
    r = pthread_create(cal_client_mdnssd_bip_thread, NULL, cal_client_mdnssd_bip_function, this);
    if (r != 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: cannot create thread: %s", strerror(errno));
        goto fail4;
    }


    // make the cal_fd non-blocking
    r = fcntl(cal_client_mdnssd_bip_fds_to_user[0], F_SETFL, O_NONBLOCK);
    if (r != 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: cannot make cal_fd nonblocking: %s", strerror(errno));
        goto fail5;
    }


    return cal_client_mdnssd_bip_fds_to_user[0];


fail5: 
    r = pthread_cancel(*cal_client_mdnssd_bip_thread);
    if (r != 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: error canceling client thread: %s", strerror(errno));
    } else {
        pthread_join(*cal_client_mdnssd_bip_thread, NULL);
        free(cal_client_mdnssd_bip_thread);
        cal_client_mdnssd_bip_thread = NULL;
    }

fail4:
    free(cal_client_mdnssd_bip_thread);
    cal_client_mdnssd_bip_thread = NULL;
    cal_client.callback = NULL;

fail3:
    close(cal_client_mdnssd_bip_fds_from_user[0]);
    close(cal_client_mdnssd_bip_fds_from_user[1]);

fail2:
    close(cal_client_mdnssd_bip_fds_to_user[0]);
    close(cal_client_mdnssd_bip_fds_to_user[1]);

fail1:
    free(this);

fail0:
    return -1;
}




void cal_client_mdnssd_bip_shutdown(void) {
    int r;

    if (cal_client_mdnssd_bip_thread == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "shutdown: called before init()!");
        return;
    }

    r = pthread_cancel(*cal_client_mdnssd_bip_thread);
    if (r != 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "shutdown: error canceling client thread: %s", strerror(errno));
        return;
    } else {
        pthread_join(*cal_client_mdnssd_bip_thread, NULL);
        free(cal_client_mdnssd_bip_thread);
        cal_client_mdnssd_bip_thread = NULL;
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

    if (cal_client_mdnssd_bip_thread == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "subscribe: called before init()!");
        return 0;
    }

    event = cal_event_new(CAL_EVENT_SUBSCRIBE);
    if (event == NULL) {
        return 0;
    }

    event->topic = strdup(topic);
    if (event->topic == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "subscribe: out of memory");
        return 0;
    }

    event->peer_name = strdup(peer_name);
    if (event->peer_name == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "subscribe: out of memory");
        cal_event_free(event);
        return 0;
    }

    r = write(cal_client_mdnssd_bip_fds_from_user[1], &event, sizeof(event));
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "subscribe: error writing to client thread: %s", strerror(errno));
        return 0;
    }
    if (r < sizeof(event)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "subscribe: short write to client thread!!");
        return 0;
    }

    return 1;
}




int cal_client_mdnssd_bip_read(void) {
    cal_event_t *event;
    int r;

    if (cal_client_mdnssd_bip_thread == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read: called before init()!");
        return 0;
    }

    r = read(cal_client_mdnssd_bip_fds_to_user[0], &event, sizeof(event));
    if (r < 0) {
        if (errno == EAGAIN) return 1;
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read: error: %s", strerror(errno));
        return 0;
    } else if (r != sizeof(event)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read: short read from client thread");
        return 0;
    } else if (event == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read: got NULL event!");
        return 0;
    }

    if (cal_client.callback != NULL) {
        cal_client.callback(event);
    }

    // manage memory
    switch (event->type) {
        case CAL_EVENT_JOIN: {
            break;
        }

        case CAL_EVENT_LEAVE: {
            break;
        }

        case CAL_EVENT_MESSAGE: {
            break;
        }

        case CAL_EVENT_PUBLISH: {
            break;
        }

        default: {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read: got unhandled event type %d", event->type);
            return 1;  // dont free events we dont understand
        }
    }

    cal_event_free(event);

    return 1;
}




int cal_client_mdnssd_bip_sendto(const char *peer_name, void *msg, int size) {
    int r;
    cal_event_t *event;

    if (cal_client_mdnssd_bip_thread == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "sendto: called before init()!");
        return 0;
    }

    event = cal_event_new(CAL_EVENT_MESSAGE);
    if (event == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "sendto: out of memory");
        return 0;
    }

    event->peer_name = strdup(peer_name);
    if (event->peer_name == NULL) {
        cal_event_free(event);
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "sendto: out of memory");
        return 0;
    }

    event->msg.buffer = msg;
    event->msg.size = size;

    r = write(cal_client_mdnssd_bip_fds_from_user[1], &event, sizeof(event));
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "sendto: error writing to client thread: %s", strerror(errno));
        return 0;
    }
    if (r < sizeof(event)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "sendto: short write to client thread!!");
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

