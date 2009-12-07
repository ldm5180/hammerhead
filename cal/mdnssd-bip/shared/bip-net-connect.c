
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "config.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

// This includes all the platform specific sock headers
// and defines some bip-socket-* warppers
#include "bip-socket-api.h"

// use errno only for setting, since errno codes on winsock are completely
// incompatible with posix
#include <errno.h>

#include "cal-mdnssd-bip.h"


#include <openssl/err.h>

extern SSL_CTX * ssl_ctx_client;
extern bip_sec_type_t client_require_security;


//
// Start the connection.
//
// returns the socket that is connect-pending, or -1 on error
// 
// Use bip_net_connect_check() after the returned fd is writable to see if it succeeded
//
int bip_net_connect_nonblock(const char* peer_name, bip_peer_network_info_t *net) {
    int s;
    int r;

    struct addrinfo ai_hints;
    struct addrinfo *ai;


    if (net == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s: NULL net passed in", __FUNCTION__);
        return -1;
    }


    if (net->socket >=0) return net->socket;

    if (client_require_security == BIP_SEC_REQ && net->sectype == BIP_SEC_NONE){
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
            "%s: Security required but not supported on peer '%s'. Refusing to connect",
                 __FUNCTION__, peer_name);
        return -1;
    }
    if (client_require_security == BIP_SEC_NONE && net->sectype == BIP_SEC_REQ){
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
            "%s: Security disabled, but required on peer '%s'. Refusing to connect",
                __FUNCTION__, peer_name);
        return -1;
    }

    s = bip_socket_tcp();
    if (s < 0) {
        return -1;
    }

    //enable keep-alives
    {
        int one = 1;
        if (setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (void*)&one, sizeof(one))) {
            g_warning("%s: error setting SO_KEEPALIVE: %m", __FUNCTION__);
        }
#ifdef LINUX
        int idle = 10, intvl = 10, cnt = 3;
        if (setsockopt(s, SOL_TCP, TCP_KEEPIDLE, (void*)&idle, sizeof(idle))) {
            g_warning("%s: error setting TCP_KEEPIDLE: %m", __FUNCTION__);
        }
        if (setsockopt(s, SOL_TCP, TCP_KEEPINTVL, (void*)&intvl, sizeof(intvl))) {
            g_warning("%s: error setting SO_KEEPINTVL: %m", __FUNCTION__);
        }
        if (setsockopt(s, SOL_TCP, TCP_KEEPCNT, (void*)&cnt, sizeof(cnt))) {
            g_warning("%s: error setting SO_KEEPCNT: %m", __FUNCTION__);
        }
#endif
    }

    // Make socket non-blocking
    bip_socket_set_blocking(s, 0);

    memset(&ai_hints, 0, sizeof(ai_hints));
    ai_hints.ai_family = AF_INET;  // IPv4
    ai_hints.ai_socktype = SOCK_STREAM;  // TCP
    r = getaddrinfo(net->hostname, NULL, &ai_hints, &ai);
    if (r != 0) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            "%s: error with getaddrinfo(\"%s\", ...): %s",
                __FUNCTION__, net->hostname, gai_strerror(r));
        return -1;
    }
    if (ai == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            "%s: no results from getaddrinfo(\"%s\", ...)", 
                __FUNCTION__, net->hostname);
        return -1;
    }

    ((struct sockaddr_in *)ai->ai_addr)->sin_port = htons(net->port);

    net->socket = s;
    net->security_status = BIP_SEC_NONE;
    if( 0 != bip_socket_connect(s, ai->ai_addr, ai->ai_addrlen, peer_name, net) ) {
        return -1;
    }

    freeaddrinfo(ai);

    return s;
}

//
// Check the status of the newly created connection. 
//
// Returns BIO on success, or NULL on error. 
// If error, check errno
//
int bip_net_connect_check(const char * peer_name, bip_peer_network_info_t *net) {

    if ( net->socket < 0 ) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "%s: socket checked before being started", __FUNCTION__);
        errno = EBADF;
        return -1;
    }

    BIO * bio;
    BIO * bio_ssl;
    if(net->pending_bio == NULL) {
        int sol_error;
        socklen_t sol_len = sizeof(sol_error);
        getsockopt(net->socket, SOL_SOCKET, SO_ERROR, &sol_error, &sol_len);

        if(sol_error != 0 ) {
#if 0
            g_log(
                CAL_LOG_DOMAIN,
                G_LOG_LEVEL_WARNING,
                "%s: error connecting to peer '%s' at %s:%hu: %s",
                __FUNCTION__,
                peer_name,
                net->hostname,
                net->port,
                strerror(sol_error)
                );
#endif

            close(net->socket);
            net->socket = -1;
            errno = sol_error;
            return -1;
        }

        
        bio = BIO_new_socket(net->socket, BIO_CLOSE);
        if (ssl_ctx_client && (net->sectype == BIP_SEC_OPT || net->sectype == BIP_SEC_REQ)) {
            bio_ssl = BIO_new_ssl(ssl_ctx_client, 1);
            if (!bio_ssl) {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Failed to create an SSL.");
                BIO_free_all(bio);
                errno = ENOMEM;
                return -1;
            }
            bio = BIO_push(bio_ssl, bio);
        }
    } else {
        bio = net->pending_bio;
    }


    if (ssl_ctx_client && (net->sectype == BIP_SEC_OPT || net->sectype == BIP_SEC_REQ)) {
	if (1 != BIO_do_handshake(bio)) {
            if ( BIO_should_retry(bio)) {
                net->pending_bio = bio;
                return 0;
            } else {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Failed to complete SSL handshake on connect: %s",
                    ERR_error_string(ERR_get_error(), NULL));
            }
	    BIO_free_all(bio);
            errno = SSL_ERROR_ERNO_VAL;
	    return -1;
	} else {
	    net->security_status = BIP_SEC_REQ;
	}
    }

    net->socket_bio = bio;
    net->pending_bio = NULL;

    return 1;
}

