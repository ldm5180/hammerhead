
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <netinet/in.h>

#include <sys/socket.h>

#include <glib.h>

#include "cal-server.h"
#include "cal-mdnssd-bip.h"
#include "cal-server-mdnssd-bip.h"




int cal_server_mdnssd_bip_init(
    const char *network_type,
    const char *name,
    void (*callback)(const cal_event_t *event),
    int (*topic_matches)(const char *a, const char *b)
) {
    int r;

    struct sockaddr_in my_address;
    socklen_t my_address_len;

    cal_server_mdnssd_bip_t *this;


    // initialize threading, if the user hasn't already
    if (!g_thread_supported()) g_thread_init(NULL);


    if (network_type == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "init: NULL network_type passed in");
        return -1;
    }

    cal_server_mdnssd_bip_network_type = strdup(network_type);
    if (cal_server_mdnssd_bip_network_type == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "init: out of memory");
        return -1;
    }


    this = (cal_server_mdnssd_bip_t *)calloc(1, sizeof(cal_server_mdnssd_bip_t));
    if (this == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "init: out of memory");
        return -1;
    }

    if (topic_matches == NULL) {
        this->topic_matches = strcmp;
    } else {
        this->topic_matches = topic_matches;
    }

    this->name = strdup(name);
    if (this->name == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "init: out of memory");
        goto fail0;
    }


    //
    // create the listening socket
    //

    this->socket = socket(PF_INET, SOCK_STREAM, 0);
    if (this->socket == -1) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: cannot create TCP socket: %s", strerror(errno));
        goto fail1;
    }


    // turn on REUSEADDR, so it'll start right back up after dying
    {
        int flag = 1;
        int r;

        r = setsockopt(this->socket, SOL_SOCKET, SO_REUSEADDR, (void*)&flag, sizeof(int));
        if (r < 0) g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: ignoring setsockopt SO_REUSEADDR error: %s", strerror(errno));
    }

    // turn on LINGER, so we can make sure all network traffic is emitted on shutdown
    {
        struct linger l;
        int r;

        l.l_onoff = 1;
        l.l_linger = 10;  // 10 seconds

        r = setsockopt(this->socket, SOL_SOCKET, SO_LINGER, (void*)&l, sizeof(l));
        if (r < 0) g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: ignoring setsockopt SO_LINGER error: %s", strerror(errno));
    }


    // ok! listen for connections
    // we dont need to bind since listen on an unbound socket defaults to INADDR_ANY and a random port, which is what we want
    r = listen(this->socket, 20);
    if (r != 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: cannot listen on port: %s", strerror(errno));
        goto fail2;
    }

    memset(&my_address, 0, sizeof(my_address));
    my_address_len = sizeof(my_address);
    r = getsockname(this->socket, (struct sockaddr *)&my_address, &my_address_len);
    if (r != 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: cannot get socket port: %s", strerror(errno));
        goto fail2;
    }

    this->port = ntohs(my_address.sin_port);


    // 
    // if we get here, the listening socket is set up and we're ready to start the publisher thread
    //


    // create the pipe for passing events back to the user
    r = pipe(cal_server_mdnssd_bip_fds_to_user);
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: error making to-user pipe: %s", strerror(errno));
        goto fail2;
    }

    // create the pipe for getting subscription requests from the user
    r = pipe(cal_server_mdnssd_bip_fds_from_user);
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: error making from-user pipe: %s", strerror(errno));
        goto fail3;
    }

    cal_server_mdnssd_bip_thread = (pthread_t *)malloc(sizeof(pthread_t));
    if (cal_server_mdnssd_bip_thread == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "init: cannot allocate memory for publisher thread: %s", strerror(errno));
        goto fail4;
    }

    // record the user's callback function
    cal_server.callback = callback;


    // start the publisher thread to talk to the peers
    r = pthread_create(cal_server_mdnssd_bip_thread, NULL, cal_server_mdnssd_bip_function, this);
    if (r != 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: cannot start publisher thread: %s", strerror(errno));
        goto fail5;
    }

    // wait to see if the CAL thread started up successfully
    // NOTE: the pipe is still blocking so this read will block until the CAL thread spins up
    {
        cal_event_t *event;
        int r;

        r = read(cal_server_mdnssd_bip_fds_to_user[0], &event, sizeof(cal_event_t*));
        if (r < 0) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: error: %s", strerror(errno));
            cal_server_mdnssd_bip_shutdown();
            return -1;
        } else if (r != sizeof(cal_event_t*)) {
            // this indicates the CAL thread refused to start up
            return -1;
        } else if (event == NULL) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: got NULL event!");
            cal_server_mdnssd_bip_shutdown();
            return -1;
        } else if (event->type != CAL_EVENT_INIT) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: got unexpected event type %d while waiting for INIT!", event->type);
            cal_server_mdnssd_bip_shutdown();
            cal_event_free(event);
            return -1;
        }

        cal_event_free(event);
    }


    // make the cal_fd non-blocking
    r = fcntl(cal_server_mdnssd_bip_fds_to_user[0], F_SETFL, O_NONBLOCK);
    if (r != 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: cannot make cal_fd nonblocking: %s", strerror(errno));
        goto fail6;
    }


    return cal_server_mdnssd_bip_fds_to_user[0];


fail6: 
    r = pthread_cancel(*cal_server_mdnssd_bip_thread);
    if (r != 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: error canceling server thread: %s", strerror(errno));
    } else {
        pthread_join(*cal_server_mdnssd_bip_thread, NULL);
        free(cal_server_mdnssd_bip_thread);
        cal_server_mdnssd_bip_thread = NULL;
    }

fail5:
    free(cal_server_mdnssd_bip_thread);
    cal_server_mdnssd_bip_thread = NULL;

fail4:
    close(cal_server_mdnssd_bip_fds_from_user[0]);
    close(cal_server_mdnssd_bip_fds_from_user[1]);

fail3:
    close(cal_server_mdnssd_bip_fds_to_user[0]);
    close(cal_server_mdnssd_bip_fds_to_user[1]);

fail2:
    close(this->socket);

fail1:
    free(this->name);

fail0: 
    free(this);
    return -1;
}




void cal_server_mdnssd_bip_shutdown(void) {
    int r;

    if (cal_server_mdnssd_bip_thread == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "shutdown: called before init!");
        return;
    }

    r = pthread_cancel(*cal_server_mdnssd_bip_thread);
    if (r != 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "shutdown: error canceling server thread: %s", strerror(errno));
        return;
    } else {
        pthread_join(*cal_server_mdnssd_bip_thread, NULL);
        free(cal_server_mdnssd_bip_thread);
        cal_server_mdnssd_bip_thread = NULL;
    }

    close(cal_server_mdnssd_bip_fds_to_user[0]);
    close(cal_server_mdnssd_bip_fds_to_user[1]);

    close(cal_server_mdnssd_bip_fds_from_user[0]);
    close(cal_server_mdnssd_bip_fds_from_user[1]);

    cal_server.callback = NULL;
}




int cal_server_mdnssd_bip_read(struct timeval *timeout) {
    int r;
    cal_event_t *event;

    if (cal_server_mdnssd_bip_thread == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read: called before init!");
        return 0;
    }

    if ((timeout == NULL) || ((timeout) && ((timeout->tv_sec > 0) || timeout->tv_usec > 0))) {
	fd_set readers;
	int ret;

	FD_ZERO(&readers);
	FD_SET(cal_server_mdnssd_bip_fds_to_user[0], &readers);
	ret = select(cal_server_mdnssd_bip_fds_to_user[0] + 1, &readers, NULL, NULL, timeout);
	if (0 > ret) {
	    if ((EAGAIN != errno)
		&& (EINTR != errno)) {
		return 0;
	    }
	    return 1;
	}
	else if (0 == ret)
	{
	    return 1;
	}
    }

    r = read(cal_server_mdnssd_bip_fds_to_user[0], &event, sizeof(cal_event_t*));
    if (r < 0) {
        if (errno == EAGAIN) return 1;
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read: error: %s", strerror(errno));
        return 0;
    } else if (r != sizeof(cal_event_t*)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read: short read of event pointer!");
        return 0;
    } else if (event == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read: got NULL event!");
        return 0;
    }

    if (cal_server.callback != NULL) {
        cal_server.callback(event);
    }

    cal_event_free(event);

    return 1;
}




int cal_server_mdnssd_bip_subscribe(const char *peer_name, const char *topic) {
    int r;
    cal_event_t *event;

    if (cal_server_mdnssd_bip_thread == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "subscribe: called before init!");
        return 0;
    }

    event = cal_event_new(CAL_EVENT_SUBSCRIBE);
    if (event == NULL) {
        return 0;
    }

    event->peer_name = strdup(peer_name);
    if (event->peer_name == NULL) {
        cal_event_free(event);
        return 0;
    }

    event->topic = strdup(topic);
    if (event->topic == NULL) {
        cal_event_free(event);
        return 0;
    }

    r = write(cal_server_mdnssd_bip_fds_from_user[1], &event, sizeof(event));
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "subscribe: error writing to server thread: %s", strerror(errno));
        return 0;
    }
    if (r < sizeof(event)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "subscribe: short write to server thread!!");
        return 0;
    }

    return 1;
}




int cal_server_mdnssd_bip_sendto(const char *peer_name, void *msg, int size) {
    int r;
    cal_event_t *event;

    if (cal_server_mdnssd_bip_thread == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "sendto: called before init!");
        return 0;
    }

    event = cal_event_new(CAL_EVENT_MESSAGE);
    if (event == NULL) {
        return 0;
    }

    event->peer_name = strdup(peer_name);
    if (event->peer_name == NULL) {
        free(event);
        return 0;
    }

    event->msg.buffer = msg;
    event->msg.size = size;

    r = write(cal_server_mdnssd_bip_fds_from_user[1], &event, sizeof(event));
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "sendto: error writing to server thread: %s", strerror(errno));
        return 0;
    }
    if (r < sizeof(event)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "sendto: short write to server thread!!");
        return 0;
    }

    return 1;
}




void cal_server_mdnssd_bip_publish(const char *topic, const void *msg, int size) {
    int r;
    cal_event_t *event;

    if (cal_server_mdnssd_bip_thread == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "publish: called before init!");
        return;
    }

    event = cal_event_new(CAL_EVENT_PUBLISH);
    if (event == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "publish: out of memory");
        return;
    }

    event->topic = strdup(topic);
    if (event->topic == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "publish: out of memory");
        return;
    }

    event->msg.buffer = malloc(size);
    if (event->msg.buffer == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "publish: out of memory");
        return;
    }
    memcpy(event->msg.buffer, msg, size);
    event->msg.size = size;

    r = write(cal_server_mdnssd_bip_fds_from_user[1], &event, sizeof(event));
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "publish: error writing to server thread: %s", strerror(errno));
        return;
    }
    if (r < sizeof(event)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "publish: short write to server thread!!");
        return;
    }
}




cal_server_t cal_server = {
    .callback = NULL,

    .init = cal_server_mdnssd_bip_init,
    .shutdown = cal_server_mdnssd_bip_shutdown,

    .read = cal_server_mdnssd_bip_read,
    .subscribe = cal_server_mdnssd_bip_subscribe,
    .sendto = cal_server_mdnssd_bip_sendto,
    .publish = cal_server_mdnssd_bip_publish
};

