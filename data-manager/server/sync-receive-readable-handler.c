
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>

#include "bionet-asn.h"
#include "bionet-data-manager.h"
#include "bdm-util.h"

static int write_data_to_fd(const void *buffer, size_t size, void * fd_void) {
    int fd = *(int*)fd_void;
    int r;

    r = write(fd, buffer, size);

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
	  "    %d bytes written %x%x", r, (int)((char*)buffer)[0], (int)((char*)buffer)[1]);

    return r;
} /* write_data_to_socket() */

static void send_ack(client_t *client, BDM_Sync_Ack_t ack_type ) {
    asn_enc_rval_t asn_r;
    BDM_Sync_Ack_t sync_ack = ack_type;

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
        "Sending ACK (%ld)...", ack_type);
    
    asn_r = der_encode(&asn_DEF_BDM_Sync_Ack, &sync_ack, 
        write_data_to_fd, &client->fd);
    if (asn_r.encoded == -1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "send_sync_ack(): error with der_encode(): %m");
    }
}

int sync_receive_readable_handler(GIOChannel *unused, GIOCondition cond, client_t *client) {
    int bytes_to_read;
    int bytes_read = 0;
    asn_dec_rval_t rval;


    if (cond & (G_IO_ERR | G_IO_HUP | G_IO_NVAL)) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "disconnect event from sync sender");
        disconnect_client(client);
        return FALSE;
    }

    client->index = 0;

    do {
	bytes_to_read = sizeof(client->buffer) - client->index;
	bytes_read = read(client->fd, &client->buffer[client->index], bytes_to_read);
	if (bytes_read < 0) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error reading from sync sender: %s", strerror(errno));
	    disconnect_client(client);
	    return FALSE;
	}
	if (bytes_read == 0) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "eof from sync sender");
	    disconnect_client(client);
	    return FALSE;
	}
	
	client->index += bytes_read;

        rval = ber_decode(NULL, 
			  &asn_DEF_BDM_Sync_Message, 
			  (void **)&client->message.sync_message, 
			  client->buffer, 
			  client->index);
        if (rval.code == RC_OK) {
	    if (client->message.sync_message->present == BDM_Sync_Message_PR_metadataMessage) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
		      "sync_receive_readable_handler(): receive Sync Metadata Message");
		handle_sync_metadata_message(client, &client->message.sync_message->choice.metadataMessage);

                send_ack(client, BDM_Sync_Ack_metadataAck);
	    } else if (client->message.sync_message->present == BDM_Sync_Message_PR_datapointsMessage) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
		      "sync_receive_readable_handler(): receive Sync Datapoints Message");
		handle_sync_datapoints_message(client, &client->message.sync_message->choice.datapointsMessage);
                send_ack(client, BDM_Sync_Ack_datapointAck);
	    } else {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		      "sync_receive_readable_handler(): unknown Sync Message choice");
	    }
	    asn_DEF_BDM_Sync_Message.free_struct(&asn_DEF_BDM_Sync_Message, client->message.sync_message, 0);
            client->message.sync_message = NULL;
        } else if (rval.code == RC_WMORE) {
            // ber_decode is waiting for more data, but so far so good
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "ber_decode: waiting for more data");
	    usleep(10000);
        } else if (rval.code == RC_FAIL) {
	    // received invalid junk
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "ber_decode failed to decode the sync sender's message");
        } else {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "unknown error with ber_decode (code=%d)", rval.code);
        }

        if (rval.consumed > 0) {
            client->index -= rval.consumed;
            memmove(client->buffer, &client->buffer[rval.consumed], client->index);
        }
    } while ((rval.consumed > 0) && (client->index > 0));

    return TRUE;
}

