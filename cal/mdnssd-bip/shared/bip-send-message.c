
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bip-socket-api.h"

#include <glib.h>

#include "bip-mdnssd-api.h"

#include "cal-mdnssd-bip.h"

/*
 * Call when a write would not block
 */
int bip_drain_pending_msgs(bip_peer_network_info_t *net) {
    int r;

    net->write_pending = 0; // Re-enable if needed
    bip_buf_t *buf = g_queue_peek_head(&net->msg_send_queue);
    if( NULL == buf ) {
        if (1 != BIO_flush(net->socket_bio)) {
            if(BIO_should_retry(net->socket_bio)){
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "bip_send_message: error flushing: timeout");
                net->write_pending = 1;
                return 0;
            } else {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_send_message: error sending");
                return -1;
            }
        }


        return 0;
    }

    while(buf) {
        int to_write = buf->size - net->curr_msg_bytes_sent;
        void * p = buf->data + net->curr_msg_bytes_sent;
        while( to_write > 0 && (r = BIO_write(net->socket_bio, p, to_write)) > 0 ) {
            net->curr_msg_bytes_sent += r;
            to_write -= r;
            p += r;
        }
        if ( to_write > 0 ) {
            if( BIO_should_retry(net->socket_bio)) {
                // Write blocked. This is ok, try later
                net->write_pending = 1;
                buf = NULL; 
                break;
            } else {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_send_message: error sending data");
                return -1;
            }
        }

        // The full buffer was sent, remove it from the pending list
        net->curr_msg_bytes_sent = 0;
        net->bytes_queued -= buf->size;
        g_queue_pop_head(&net->msg_send_queue);
        free(buf->data); free(buf);

        buf = g_queue_peek_head(&net->msg_send_queue);

        // A complete protocol unit was sent, so send a flush too
        if (1 != BIO_flush(net->socket_bio)) {
            if(BIO_should_retry(net->socket_bio)){
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "bip_send_message: error flushing: timeout");
                net->write_pending = 1; // Send more data, or flush again when the socket becomes
            } else {
                g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_send_message: error sending");
                return -1;
            }
        }
    }

    return 0;
}

//
// TODO: Pass in a reference-counted buffer that is shared between each peer that its being 
// sent to.
//
// We're currently not limiting the size of BIP messages.  This sometimes
// worries Coverity.
//
// Maybe in the future we'll limit max message sizes in some configurable
// way, as part of a security mechanism to try to resist DOS attacks.
//
// FIXME: Also see Issue #343 at <http://bioserve.colorado.edu/issue-tracker/?module=issues&action=view&issueid=343>
//
// coverity[ -tainted_data_sink : arg-4 ]
int bip_send_message(const bip_peer_t *peer, uint8_t msg_type, const void *msg, uint32_t size) {
    uint32_t msg_size;
    bip_peer_network_info_t *net = NULL;

    if (peer == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_send_message: NULL peer passed in");
        return -1;
    }

    net = bip_peer_get_connected_net(peer);
    if (net == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_send_message: no connection to peer '%s'", peer->peer_name);
        return -1;
    }

    if( bip_drain_pending_msgs(net) < 0 ) {
        return -1;
    }

    // Check for available space..
    // We allow one msg to exceed the maximum, so we check size before
    // adding this msg
    if ( net->bytes_queued > bip_shared_cfg.max_write_buf_size ) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_send_message(%s:%hu): error sending data: Write buffer full",
                net->hostname, net->port);
        return -1;
    }

    // Create a buffer, and push it onto the queue.
    bip_buf_t *buf = malloc(sizeof(bip_buf_t));
    if (buf == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_send_message: Out of memory!");
        return -1;
    }

    buf->data = malloc(sizeof(msg_type) + sizeof(msg_size) + size);
    if (buf->data == NULL) {
        free(buf);
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_send_message: Out of memory!");
        return -1;
    }

    msg_size = htonl(size);

    memcpy(buf->data, &msg_type, sizeof(msg_type));
    memcpy(buf->data + sizeof(msg_type), &msg_size, sizeof(msg_size));
    memcpy(buf->data + sizeof(msg_type) + sizeof(msg_size), msg, size);
    buf->size = sizeof(msg_type) + sizeof(msg_size) + size;

    g_queue_push_tail(&net->msg_send_queue, buf);
    net->bytes_queued += buf->size;

    // Try now to write...
    return bip_drain_pending_msgs(net);
}

