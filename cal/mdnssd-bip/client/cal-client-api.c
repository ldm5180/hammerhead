
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "cal-client.h"
#include "cal-mdnssd-bip.h"
#include "cal-client-mdnssd-bip.h"


void * cal_client_mdnssd_bip_init(
    const char *network_type,
    void (*callback)(void * cal_handle, const cal_event_t *event),
    int (*peer_matches)(const char *peer_name, const char *subscription)
) {
    int r;
    GError *err = NULL;
    cal_client_mdnssd_bip_t * client_thread_data = NULL;

    // set up the context
    client_thread_data = (cal_client_mdnssd_bip_t *)calloc(1, sizeof(cal_client_mdnssd_bip_t));
    if (client_thread_data == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "init: out of memory!");
        goto fail0;
    }

    bip_shared_config_init();

    // initialize threading, if the user hasn't already
    if (!g_thread_supported()) g_thread_init(NULL);


    if (network_type == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "init: NULL network_type specified!");
        goto fail0;
    }

    client_thread_data->cal_client_mdnssd_bip_network_type = strdup(network_type);
    if (client_thread_data->cal_client_mdnssd_bip_network_type == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "init: out of memory!");
        goto fail0;
    }


    if (peer_matches == NULL) {
        client_thread_data->peer_matches = strcmp;
    } else {
        client_thread_data->peer_matches = peer_matches;
    }

    // create the pipe for passing events back to the user thread
    r = pipe(client_thread_data->cal_client_mdnssd_bip_fds_to_user);
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: error making to-user pipe: %s", strerror(errno));
        goto fail2;
    }

    // create the pipe for getting subscription requests from the user
    r = pipe(client_thread_data->cal_client_mdnssd_bip_fds_from_user);
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: error making from-user pipe: %s", strerror(errno));
        goto fail3;
    }


    // make the cal_fd non-blocking
    r = fcntl(client_thread_data->cal_client_mdnssd_bip_fds_to_user[0], F_SETFL, O_NONBLOCK);
    if (r != 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: cannot make cal_fd nonblocking: %s", strerror(errno));
        goto fail3;
    }

    // record the user's callback function
    cal_client.callback = callback;

    // Create and start the client thread
    client_thread_data->client_thread = g_thread_create(cal_client_mdnssd_bip_function,
						     client_thread_data,
						     TRUE,
						     &err);

    if ( client_thread_data->client_thread == NULL ) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: cannot create thread: %s", err->message);
        g_error_free(err);
        goto fail4;
    }


    return (void *)client_thread_data;


fail4:
    client_thread_data->client_thread = NULL;
    cal_client.callback = NULL;

    close(client_thread_data->cal_client_mdnssd_bip_fds_from_user[0]);
    close(client_thread_data->cal_client_mdnssd_bip_fds_from_user[1]);

fail3:
    close(client_thread_data->cal_client_mdnssd_bip_fds_to_user[0]);
    close(client_thread_data->cal_client_mdnssd_bip_fds_to_user[1]);

fail2:
    free(client_thread_data->cal_client_mdnssd_bip_network_type);
    cal_client_mdnssd_bip_thread_destroy(client_thread_data);

fail0:
    return NULL;
}



static void _eat_messages_until_shutdown(void * cal_handle) {
    cal_client_mdnssd_bip_t * this = (cal_client_mdnssd_bip_t *)cal_handle;
    char buf[1024];

    while(read(this->cal_client_mdnssd_bip_fds_to_user[0], buf, sizeof(buf)) > 0 );

}

void cal_client_mdnssd_bip_shutdown(void * cal) {
    cal_client_mdnssd_bip_t * this = (cal_client_mdnssd_bip_t *)cal;
    if (this->client_thread == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "shutdown: called before init()!");
        return;
    }

    close(this->cal_client_mdnssd_bip_fds_from_user[1]);
    this->cal_client_mdnssd_bip_fds_from_user[1] = -1;

    _eat_messages_until_shutdown(this);

    g_thread_join(this->client_thread);

    close(this->cal_client_mdnssd_bip_fds_to_user[0]);
    close(this->cal_client_mdnssd_bip_fds_to_user[1]);
    close(this->cal_client_mdnssd_bip_fds_from_user[0]);
    close(this->cal_client_mdnssd_bip_fds_from_user[1]);

    cal_client.callback = NULL;

    if (this->ssl_ctx_client) {
	SSL_CTX_free(this->ssl_ctx_client);
    }

    // Thread has exited, clean up its state
    cal_client_mdnssd_bip_thread_destroy(this);
}




int cal_client_mdnssd_bip_subscribe(void * cal_handle,
				    const char *peer_name, 
				    const char *topic) {
    cal_client_mdnssd_bip_t * this = (cal_client_mdnssd_bip_t *)cal_handle;
    int r;
    cal_event_t *event;

    if (this->client_thread == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "subscribe: called before init()!");
        return 0;
    }

    if (!cal_peer_name_is_valid(peer_name)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "subscribe: called with invalid peer_name!");
        return 0;
    }

    if (!cal_topic_is_valid(topic)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "subscribe: called with invalid topic!");
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

    r = write(this->cal_client_mdnssd_bip_fds_from_user[1], &event, sizeof(event));
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "subscribe: error writing to client thread: %s", strerror(errno));
        cal_event_free(event);
        return 0;
    }
    if (r < sizeof(event)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "subscribe: short write to client thread!!");
        cal_event_free(event);
        return 0;
    }

    // 'event' passes out of scope here, but we don't leak its memory
    // because we have successfully sent a pointer to it to the user thread
    // coverity[leaked_storage]
    return 1;
}




int cal_client_mdnssd_bip_unsubscribe(void * cal_handle,
				      const char *peer_name, 
				      const char *topic) {
    cal_client_mdnssd_bip_t * this = (cal_client_mdnssd_bip_t *)cal_handle;
    int r;
    cal_event_t *event;

    if (this->client_thread == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "unsubscribe: called before init()!");
        return 0;
    }

    if (!cal_peer_name_is_valid(peer_name)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "unsubscribe: called with invalid peer_name!");
        return 0;
    }

    if (!cal_topic_is_valid(topic)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "unsubscribe: called with invalid topic!");
        return 0;
    }

    event = cal_event_new(CAL_EVENT_UNSUBSCRIBE);
    if (event == NULL) {
        return 0;
    }

    event->topic = strdup(topic);
    if (event->topic == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "unsubscribe: out of memory");
        return 0;
    }

    event->peer_name = strdup(peer_name);
    if (event->peer_name == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "unsubscribe: out of memory");
        cal_event_free(event);
        return 0;
    }

    r = write(this->cal_client_mdnssd_bip_fds_from_user[1], &event, sizeof(event));
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "unsubscribe: error writing to client thread: %s", strerror(errno));
        cal_event_free(event);
        return 0;
    }
    if (r < sizeof(event)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "unsubscribe: short write to client thread!!");
        cal_event_free(event);
        return 0;
    }

    // 'event' passes out of scope here, but we don't leak its memory
    // because we have successfully sent a pointer to it to the user thread
    // coverity[leaked_storage]
    return 1;
}




int cal_client_mdnssd_bip_read(void * cal_handle, struct timeval * timeout) {
    cal_client_mdnssd_bip_t * this = (cal_client_mdnssd_bip_t *)cal_handle;
    cal_event_t *event;
    int r;

    if (this->client_thread == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read: called before init()!");
        return 0;
    }

    if ((timeout == NULL) || ((timeout) && ((timeout->tv_sec > 0) || timeout->tv_usec > 0))) {
	fd_set readers;
	int ret;

	FD_ZERO(&readers);
	FD_SET(this->cal_client_mdnssd_bip_fds_to_user[0], &readers);
	ret = select(this->cal_client_mdnssd_bip_fds_to_user[0] + 1, &readers, NULL, NULL, timeout);
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

    r = read(this->cal_client_mdnssd_bip_fds_to_user[0], &event, sizeof(event));
    if (r < 0) {
        if (errno == EAGAIN) return 1;
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read: error: %s", strerror(errno));
        return 0;
    } else if (r != sizeof(event)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read: short read from client thread");
        return 0;
    } else if (event == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read: ignoring NULL event from CAL Client thread!");
        return 0;
    }

    if (cal_client.callback != NULL) {
        cal_client.callback(this, event);
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

        case CAL_EVENT_SUBSCRIBE: {
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




int cal_client_mdnssd_bip_sendto(void * cal_handle,
				 const char *peer_name, 
				 void *msg, 
				 int size) {
    cal_client_mdnssd_bip_t * this = (cal_client_mdnssd_bip_t *)cal_handle;
    int r;
    cal_event_t *event;

    if (this->client_thread == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "sendto: called before init()!");
        return 0;
    }

    if (!cal_peer_name_is_valid(peer_name)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "sendto: called with invalid peer_name!");
        return 0;
    }

    if (msg == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "sendto: NULL msg passed in");
        return 0;
    }

    if (size < 1) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "sendto: invalid size %d passed in", size);
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

    r = write(this->cal_client_mdnssd_bip_fds_from_user[1], &event, sizeof(event));
    if (r < 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "sendto: error writing to client thread: %s", strerror(errno));
        cal_event_free(event);
        return 0;
    }
    if (r < sizeof(event)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "sendto: short write to client thread!!");
        cal_event_free(event);
        return 0;
    }

    // 'event' passes out of scope here, but we don't leak its memory
    // because we have successfully sent a pointer to it to the user thread
    // coverity[leaked_storage]
    return 1;
}


int cal_client_mdnssd_bip_init_security(void * cal_handle, const char * dir, int require) {
    cal_client_mdnssd_bip_t * this = (cal_client_mdnssd_bip_t *)cal_handle;
    char cadir[1024];
    char pubcert[1024];
    char prvkey[1024];
    int r;

    if (NULL == dir) {
	g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "cal_server_mdnssd_bip_init_security(): NULL dir passed in.");
	return 0;
    }

    r = snprintf(cadir, 1024, "%s/%s", dir, BIP_CA_DIR);
    if (0 >= r || r == 1024) {
	if (require) {
	    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
		  "Failed to make CA Dir variable.");
	    return 0;
	} else {
	    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Failed to make CA Dir variable.");
	    return 1;
	}
    }

    r = snprintf(pubcert, 1024, "%s/%s", dir, BIP_PUBLIC_CERT);
    if (0 >= r || r == 1024) {
	if (require) {
	    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
		  "Failed to make CA Cert variable.");
	    return 0;
	} else {
	    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Failed to make CA Cert variable.");
	    return 1;
	}
    }

    r = snprintf(prvkey, 1024, "%s/%s", dir, BIP_PRIVATE_KEY);
    if (0 >= r || r == 1024) {
	if (require) {
	    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
		  "Failed to make Private Key variable.");
	    return 0;
	} else {
	    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Failed to make Private Key variable.");
	    return 1;
	}
    }

    SSL_load_error_strings();

    //seed the PRNG
    if (!RAND_load_file("/dev/urandom", 1024)) {
	if (require) {
	    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		  "Failed to init PRNG.");
	    return 0;
	} else {
	    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Failed to init PRNG - continuing without security.");
	    return 1;
	}
    }

    
    SSLeay_add_ssl_algorithms();
    this->ssl_ctx_client = SSL_CTX_new(SSLv23_client_method());

    //verify the SSL dir
    if (1 != SSL_CTX_load_verify_locations(this->ssl_ctx_client, BIP_CA_FILE, cadir)) {
	ERR_print_errors_fp(stderr);
	if (require) {
	    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		  "Failed to load CA directory.");
	    return 0;
	} else {
	    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Failed to load CA directory - continuing without security.");
	    return 1;
	}
    }

    //load the trusted CA list
    if (1 != SSL_CTX_use_certificate_chain_file(this->ssl_ctx_client, pubcert)) {
	ERR_print_errors_fp(stderr);
	if (require) {
	    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		  "Failed to load certificate.");
	    return 0;
	} else {
	    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Failed to load certificate - continuing without security.");
	    return 1;
	}
    }

    //load the private key
    if (1 != SSL_CTX_use_PrivateKey_file(this->ssl_ctx_client, prvkey, SSL_FILETYPE_PEM)) {
	ERR_print_errors_fp(stderr);
	if (require) {
	    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		  "Failed to load private key.");
	    return 0;
	} else {
	    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Failed to load private key - continuing without security.");
	    return 1;
	}
    }

    SSL_CTX_set_verify(this->ssl_ctx_client, 
		       SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, 
		       bip_ssl_verify_callback);
    SSL_CTX_set_verify_depth(this->ssl_ctx_client, 9); //arbitrary right now

    if(require) {
	this->client_require_security = BIP_SEC_REQ;
    } else {
	this->client_require_security = BIP_SEC_OPT;
    }
    return 1;
} /* cal_client_mdnssd_bip_init_security() */


int cal_client_mdnssd_bip_get_fd(void * cal_handle) {
    cal_client_mdnssd_bip_t * this = (cal_client_mdnssd_bip_t *)cal_handle;
    if (NULL == this) {
	g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "NULL CAL Handle passed in!");
	return -1;
    }
    
    return this->cal_client_mdnssd_bip_fds_to_user[0];
} /* cal_client_mdnssd_bip_get_fd() */


cal_client_t cal_client = {
    .callback = NULL,

    .init = cal_client_mdnssd_bip_init,
    .shutdown = cal_client_mdnssd_bip_shutdown,

    .subscribe = cal_client_mdnssd_bip_subscribe,
    .unsubscribe = cal_client_mdnssd_bip_unsubscribe,

    .read = cal_client_mdnssd_bip_read,

    .sendto = cal_client_mdnssd_bip_sendto,

    .init_security = cal_client_mdnssd_bip_init_security,
    
    .get_fd = cal_client_mdnssd_bip_get_fd
};

