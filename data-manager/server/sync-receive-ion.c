
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "config.h"

#if ENABLE_ION

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include <sys/time.h>

#include "bionet-asn.h"
#include "bionet-data-manager.h"

#include "bps/bps_socket.h"

#include "bdm-stats.h"

int send_ack_bundle(int bpfd, BDM_Sync_Message_t * sync_msg) {

    bionet_asn_buffer_t buf;

    if ( bdm_gen_sync_msg_ack_asnbuf(sync_msg, &buf) ) {
        return -1;
    }

    ssize_t bytes = bps_send(bpfd, buf.buf, buf.size, 0);

    if(bytes != buf.size) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            "Failed to send sync message ACK: %s", strerror(errno));
        return -1;
    }

    num_sync_acks_sent++;

    return 0;
}

BDM_Sync_Message_t * handle_sync_bundle(int bundle_fd)
{
    int bytes_to_read;
    int bytes_read;
    int r;
    asn_dec_rval_t rval = {0};
    BDM_Sync_Message_t * sync_message = NULL;

    int buffer_index = 0;
    void * buffer = NULL;
    size_t buffer_size = 0;

    sync_message = NULL;

    for(;;) {
        bytes_to_read = buffer_size - buffer_index;
        if(bytes_to_read <= 0)
        {
            // Last decode made no progress. Increase our buffer size.
            buffer_size += 1024;
            buffer = realloc(buffer, buffer_size);
            bytes_to_read = buffer_size - buffer_index;
        }

        bytes_read = bps_recv(bundle_fd, (void*)(buffer+buffer_index), bytes_to_read, 0);
        if(bytes_read < 0)
        {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                "Error reading from accepted socket: %s", strerror(errno));
            goto fail;
        }
        if(bytes_read == 0)
        {
            // Ran out of bundle bytes...
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "Not enough data in bundle for sync message decode");
            goto fail;
        }

        buffer_index += bytes_read;

        rval = ber_decode(NULL, 
                          &asn_DEF_BDM_Sync_Message, 
                          (void **)&sync_message, 
                          buffer, 
                          buffer_index);
        switch ( rval.code ) {
            case RC_OK:
            {
                char buf[4];
                if( 0 != bps_recv(bundle_fd, buf, sizeof(buf), 0)) {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                          "sync_receive_ion(): More data present in bundle after ASN.1 message");
                    goto fail;
                }

                r = handle_sync_msg(sync_message);
                if ( r ) {
                    goto fail;
                }
                if(!sync_message_is_ack(sync_message) && sync_message->syncchannel >= 0){
                    send_ack_bundle(bundle_fd, sync_message);
                }
                goto done;
            }

            case RC_WMORE:
                // ber_decode is waiting for more data, suck more data
                // from bundle
                break;

            case RC_FAIL:
                // received invalid junk
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                    "ber_decode failed to decode the sync sender's message");
                goto fail;

            default:
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                    "unknown error with ber_decode (code=%d)", rval.code);
                goto fail;
        }

        if (rval.consumed > 0) {
            buffer_index -= rval.consumed;
            memmove(buffer, buffer + rval.consumed, buffer_index);
        }
    } 

fail:
    if(sync_message){
        asn_DEF_BDM_Sync_Message.free_struct(
            &asn_DEF_BDM_Sync_Message,
            sync_message, 0);
        sync_message = NULL;
    }

done:
    bps_close(bundle_fd);

    free(buffer);

    return sync_message;

}

#endif // ENABLE_ION
