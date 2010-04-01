
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "cal-server.h"
#include "cal-mdnssd-bip.h"
#include "cal-server-mdnssd-bip.h"
#include "shared/bip-socket-api.h"

#include "bip-socket-api.h"

void * cal_server_mdnssd_bip_init(const char *network_type,
				  const char *name,
				  void (*callback)(void * cal_handle, const cal_event_t *event),
				  int (*topic_matches)(const char *a, const char *b),
				  void * ssl_ctx,
				  int require_security)
{
    int r;

    struct sockaddr_in my_address;
    socklen_t my_address_len;
    GError *err = NULL;
    cal_server_mdnssd_bip_t * server_thread_data;

    server_thread_data = (cal_server_mdnssd_bip_t *)calloc(1, sizeof(cal_server_mdnssd_bip_t));
    if (server_thread_data == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "init: out of memory");
	goto fail0;
    }

    server_thread_data->ssl_ctx_server = (SSL_CTX *)ssl_ctx;
    if(require_security) {
	server_thread_data->server_require_security = BIP_SEC_REQ;
    } else {
	server_thread_data->server_require_security = BIP_SEC_OPT;
    }

    bip_shared_config_init();

    // initialize threading, if the user hasn't already
    if (!g_thread_supported()) g_thread_init(NULL);


    if (network_type == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "init: NULL network_type passed in");
	goto fail0;
    }

    if (!cal_peer_name_is_valid(name)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "init: invalid name passed in");
	goto fail0;
    }

    server_thread_data->cal_server_mdnssd_bip_network_type = strdup(network_type);
    if (server_thread_data->cal_server_mdnssd_bip_network_type == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "init: out of memory");
	goto fail0;
    }

    if (topic_matches == NULL) {
        server_thread_data->topic_matches = strcmp;
    } else {
        server_thread_data->topic_matches = topic_matches;
    }

    server_thread_data->name = strdup(name);
    if (server_thread_data->name == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "init: out of memory");
        goto fail0;
    }


    //
    // create the listening socket
    //

    server_thread_data->socket = bip_socket_tcp();
    if (server_thread_data->socket == -1) {
        goto fail1;
    }


    // turn on REUSEADDR, so it'll start right back up after dying
    {
        int flag = 1;
        int r;

        r = setsockopt(server_thread_data->socket, SOL_SOCKET, SO_REUSEADDR, (void*)&flag, sizeof(int));
        if (r < 0) g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: ignoring setsockopt SO_REUSEADDR error: %s", strerror(errno));
    }

    // turn on LINGER, so we can make sure all network traffic is emitted on shutdown
    {
        struct linger l;
        int r;

        l.l_onoff = 1;
        l.l_linger = 10;  // 10 seconds

        r = setsockopt(server_thread_data->socket, SOL_SOCKET, SO_LINGER, (void*)&l, sizeof(l));
        if (r < 0) g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: ignoring setsockopt SO_LINGER error: %s", strerror(errno));
    }


    // ok! listen for connections
    // we dont need to bind since listen on an unbound socket defaults to INADDR_ANY and a random port, which is what we want
    r = bip_socket_listen(server_thread_data->socket, 20);
    if (r != 0) {
        goto fail2;
    }

    memset(&my_address, 0, sizeof(my_address));
    my_address_len = sizeof(my_address);
    r = getsockname(server_thread_data->socket, (struct sockaddr *)&my_address, &my_address_len);
    if (r != 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: cannot get socket port: %s", strerror(errno));
        goto fail2;
    }

    server_thread_data->port = ntohs(my_address.sin_port);


    // 
    // if we get here, the listening socket is set up and we're ready to start the publisher thread
    //


    // create the msg-queue for passing events back to the user
    r = bip_msg_queue_init(&server_thread_data->bip_server_msgq);
    if ( r < 0 ) {
        goto fail4;
    }

    // record the user's callback function
    server_thread_data->callback = callback;

    server_thread_data->server_thread = g_thread_create(cal_server_mdnssd_bip_function, 
							server_thread_data,
							TRUE,
							&err);
    if ( server_thread_data->server_thread == NULL ) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                ID "init: cannot start publisher thread: %s", err->message);
        g_error_free(err);
        goto fail4;
    }

    // wait to see if the CAL thread started up successfully
    // NOTE: the pipe is still blocking so this read will block until the CAL thread spins up
    {
        cal_event_t *event;
        int r;

        r = bip_msg_queue_pop(&server_thread_data->bip_server_msgq, BIP_MSG_QUEUE_TO_USER, &event);
        if (r != 0) {
            cal_server_mdnssd_bip_shutdown(server_thread_data);
            return NULL;
        } else if (event == NULL) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: ignoring NULL event from CAL Server thread!");
            cal_server_mdnssd_bip_shutdown(server_thread_data);
            return NULL;
        } else if (event->type != CAL_EVENT_INIT) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "init: got unexpected event type %d while waiting for INIT!", event->type);
            cal_server_mdnssd_bip_shutdown(server_thread_data);
            cal_event_free(event);
            return NULL;
        }

        cal_event_free(event);
    }

    return server_thread_data; 

fail4:
    server_thread_data->server_thread = NULL;
    server_thread_data->callback = NULL;

    bip_msg_queue_unref(&server_thread_data->bip_server_msgq);

fail2:
    close(server_thread_data->socket);

fail1:
    free(server_thread_data->name);

fail0: 
    free(server_thread_data);
    return NULL;
}




void cal_server_mdnssd_bip_shutdown(void * cal_handle) {
    cal_event_t *event;
    int r;

    cal_server_mdnssd_bip_t * this = (cal_server_mdnssd_bip_t *)cal_handle;
    if (this->server_thread == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "shutdown: called before init!");
        return;
    }

    //
    // first tell the CAL thread to shut itself down
    //
    r = bip_msg_queue_close(&this->bip_server_msgq, BIP_MSG_QUEUE_FROM_USER);
    if (r < 0) {
        return;
    }

    //
    // read and handle any pending events from the CAL thread,
    // until it closes the pipe
    //

    while(1) {
        fd_set readers;
        int r;
        int hdl;

        hdl = bip_msg_queue_get_handle(&this->bip_server_msgq, BIP_MSG_QUEUE_TO_USER);
        FD_ZERO(&readers);
        FD_SET(hdl, &readers);

        r = select(hdl + 1, &readers, NULL, NULL, NULL);
        if (r < 0) {
            if ((errno == EAGAIN) || (errno == EINTR)) continue;
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "shutdown: error with select: %s", strerror(errno));
            return;
        }

        r = bip_msg_queue_pop(&this->bip_server_msgq, BIP_MSG_QUEUE_TO_USER, &event);
        if (r != 0) {
            // done!
            break;
        } else if (event == NULL) {
            g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "shutdown: got NULL event!");
            break;
        }

        if (this->callback != NULL) {
            this->callback(this, event);
        }

        cal_event_free(event);
    }


    //
    // when we get here, the CAL thread has closed the pipe
    //

    g_thread_join(this->server_thread);
    this->server_thread = NULL;

    bip_msg_queue_unref(&this->bip_server_msgq);

    this->callback = NULL;

    if (this->ssl_ctx_server) {
	SSL_CTX_free(this->ssl_ctx_server);
	this->ssl_ctx_server = NULL;
    }

    cal_server_mdnssd_bip_destroy(this);
}




int cal_server_mdnssd_bip_read(void * cal_handle, struct timeval *timeout) {
    int r;
    cal_event_t *event;

    cal_server_mdnssd_bip_t * this = (cal_server_mdnssd_bip_t *)cal_handle;

    if (this->server_thread == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read: called before init!");
        return 0;
    }

    {
	fd_set readers;
	int ret;

        int hdl = bip_msg_queue_get_handle(&this->bip_server_msgq, BIP_MSG_QUEUE_TO_USER);
        if ( hdl < 0 ) {
            return 0;
        }
	FD_ZERO(&readers);
	FD_SET(hdl, &readers);
	ret = select(hdl + 1, &readers, NULL, NULL, timeout);
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

    r = bip_msg_queue_pop(&this->bip_server_msgq, BIP_MSG_QUEUE_TO_USER, &event);
    if (r != 0) {
        return 0;
    } else if (event == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "read: ignoring NULL event from CAL Server thread!");
        return 0;
    }

    if (this->callback != NULL) {
	this->callback(this, event);
    }

    cal_event_free(event);

    return 1;
}




int cal_server_mdnssd_bip_subscribe(void * cal_handle,
				    const char *peer_name, 
				    const char *topic) {
    int r;
    cal_event_t *event;

    cal_server_mdnssd_bip_t * this = (cal_server_mdnssd_bip_t *)cal_handle;

    if (this->server_thread == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "subscribe: called before init!");
        return 0;
    }

    if (!cal_peer_name_is_valid(peer_name)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "subscribe: invalid peer_name");
        return 0;
    }

    if (!cal_topic_is_valid(topic)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "subscribe: invalid topic");
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

    r = bip_msg_queue_push(&this->bip_server_msgq, BIP_MSG_QUEUE_FROM_USER, event);
    if (r < 0) {
        cal_event_free(event);
        return 0;
    }

    // 'event' passes out of scope here, but we don't leak its memory
    // because we have successfully sent a pointer to it to the user thread
    // coverity[leaked_storage]
    return 1;
}




int cal_server_mdnssd_bip_sendto(void * cal_handle,
				 const char *peer_name, 
				 void *msg, 
				 int size) {
    int r;
    cal_event_t *event;

    cal_server_mdnssd_bip_t * this = (cal_server_mdnssd_bip_t *)cal_handle;

    if (this->server_thread == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "sendto: called before init!");
        return 0;
    }

    if (!cal_peer_name_is_valid(peer_name)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "sendto: invalid peer_name!");
        return 0;
    }

    if (msg == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "sendto: called with NULL msg!");
        return 0;
    }

    if (size < 1) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "sendto: called with invalid size %d!", size);
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

    r = bip_msg_queue_push(&this->bip_server_msgq, BIP_MSG_QUEUE_FROM_USER, event);
    if (r < 0) {
        cal_event_free(event);
        return 0;
    }

    // 'event' passes out of scope here, but we don't leak its memory
    // because we have successfully sent a pointer to it to the user thread
    // coverity[leaked_storage]
    return 1;
}




void cal_server_mdnssd_bip_publish(void * cal_handle,
				   const char *topic, 
				   const void *msg, 
				   int size) {
    int r;
    cal_event_t *event;

    cal_server_mdnssd_bip_t * this = (cal_server_mdnssd_bip_t *)cal_handle;

    if (this->server_thread == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "publish: called before init!");
        return;
    }

    if (!cal_topic_is_valid(topic)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "publish: invalid topic");
        return;
    }

    if (msg == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "publish: called with NULL msg!");
        return;
    }

    if (size < 1) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "publish: called with invalid size %d!", size);
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

    r = bip_msg_queue_push(&this->bip_server_msgq, BIP_MSG_QUEUE_FROM_USER, event);
    if (r < 0) {
        cal_event_free(event);
        return;
    }

    // 'event' passes out of scope here, but we don't leak its memory
    // because we have successfully sent a pointer to it to the user thread
    // coverity[leaked_storage]
    return;
}


void cal_server_mdnssd_bip_publishto(void * cal_handle,
				     const char *peer_name, 
				     const char *topic, 
				     const void *msg, 
				     int size) {
    int r;
    cal_event_t *event;

    cal_server_mdnssd_bip_t * this = (cal_server_mdnssd_bip_t *)cal_handle;

    if (this->server_thread == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "publish: called before init!");
        return;
    }

    if (!cal_topic_is_valid(topic)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "publish: invalid topic");
        return;
    }

    if (!cal_peer_name_is_valid(peer_name)) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "subscribe: invalid peer_name");
        return;
    }


    if (msg == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "publish: called with NULL msg!");
        return;
    }

    if (size < 1) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "publish: called with invalid size %d!", size);
        return;
    }

    event = cal_event_new(CAL_EVENT_PUBLISH);
    if (event == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ID "publish: out of memory");
        return;
    }

    event->peer_name = strdup(peer_name);
    if (event->peer_name == NULL) {
        free(event);
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

    r = bip_msg_queue_push(&this->bip_server_msgq, BIP_MSG_QUEUE_FROM_USER, event);
    if (r < 0) {
        cal_event_free(event);
        return;
    }

    // 'event' passes out of scope here, but we don't leak its memory
    // because we have successfully sent a pointer to it to the user thread
    // coverity[leaked_storage]
    return;
}


void * cal_server_mdnssd_bip_init_security(const char * dir, int require) {
    char cadir[1024];
    char pubcert[1024];
    char prvkey[1024];
    int r;
    SSL_CTX * ssl_ctx_server;

    if (NULL == dir) {
	g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "cal_server_mdnssd_bip_init_security(): NULL dir passed in.");
	return 0;
    }

    r = snprintf(cadir, 1024, "%s/%s", dir, BIP_CA_DIR);
    if (0 >= r || r >= 1024) {
	if (require) {
	    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
		  "Failed to make CA Dir variable.");
	    return NULL;
	}
    }

    r = snprintf(pubcert, 1024, "%s/%s", dir, BIP_PUBLIC_CERT);
    if (0 >= r || r >= 1024) {
	if (require) {
	    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
		  "Failed to make CA Cert variable.");
	    return NULL;
	}
    }

    r = snprintf(prvkey, 1024, "%s/%s", dir, BIP_PRIVATE_KEY);
    if (0 >= r || r >= 1024) {
	if (require) {
	    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
		  "Failed to make Private Key variable.");
	    return NULL;
	}
    }

    SSL_load_error_strings();

    //seed the PRNG
    if (!RAND_load_file("/dev/urandom", 1024)) {
	if (require) {
	    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		  "Failed to init PRNG.");
	    return NULL;
	}
    }

    SSLeay_add_ssl_algorithms();
    ssl_ctx_server = SSL_CTX_new(SSLv23_server_method());
    if (NULL == ssl_ctx_server) {
	ERR_print_errors_fp(stderr);
	if (require) {
	    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		  "Failed to get a new SSL context");
	    return NULL;
	}
    }
 
    //verify the SSL dir
    if (1 != SSL_CTX_load_verify_locations(ssl_ctx_server, NULL, cadir)) {
	ERR_print_errors_fp(stderr);
	if (require) {
	    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		  "Failed to load CA directory.");
	    goto security_fail;
	}
    }

    //load the trusted CA list
    if (1 != SSL_CTX_use_certificate_chain_file(ssl_ctx_server, pubcert)) {
	ERR_print_errors_fp(stderr);
	if (require) {
	    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		  "Failed to load certificate.");
	    goto security_fail;
	}
    }

    //load the private key
    if (1 != SSL_CTX_use_PrivateKey_file(ssl_ctx_server, prvkey, SSL_FILETYPE_PEM)) {
	ERR_print_errors_fp(stderr);
	if (require) {
	    g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
		  "Failed to load private key.");
	    goto security_fail;
	}
    }

    SSL_CTX_set_verify(ssl_ctx_server, 
		       SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, 
		       bip_ssl_verify_callback);
    SSL_CTX_set_verify_depth(ssl_ctx_server, 9); //arbitrary right now

    return ssl_ctx_server;

//security failed
security_fail:
    SSL_CTX_free(ssl_ctx_server);
    ssl_ctx_server = NULL;
    return NULL;
} /* cal_server_mdnssd_bip_init_security() */

int cal_server_mdnssd_bip_get_fd(void * cal_handle) {
    cal_server_mdnssd_bip_t * this = (cal_server_mdnssd_bip_t *)cal_handle;
    if (NULL == this) {
	g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, ID "NULL CAL Handle passed in!");
	return -1;
    }
    
    return bip_msg_queue_get_handle(&this->bip_server_msgq, BIP_MSG_QUEUE_TO_USER);
} /* cal_server_mdnssd_bip_get_fd() */

cal_server_t cal_server = {
    .init = cal_server_mdnssd_bip_init,
    .shutdown = cal_server_mdnssd_bip_shutdown,

    .read = cal_server_mdnssd_bip_read,
    .subscribe = cal_server_mdnssd_bip_subscribe,
    .sendto = cal_server_mdnssd_bip_sendto,
    .publish = cal_server_mdnssd_bip_publish,
    .publishto = cal_server_mdnssd_bip_publishto,

    .init_security = cal_server_mdnssd_bip_init_security,
    .get_fd = cal_server_mdnssd_bip_get_fd
};

