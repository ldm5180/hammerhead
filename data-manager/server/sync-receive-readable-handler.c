
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>

#include "bionet-asn.h"
#include "bionet-data-manager.h"

static int write_data_to_fd(const void *buffer, size_t size, void * fd_void) {
    int fd = *(int*)fd_void;
    int r;

    r = write(fd, buffer, size);

    /*
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
	  "    %d bytes written %x%x", 
          r, (int)((char*)buffer)[0], (int)((char*)buffer)[1]);
    */

    return r;
} /* write_data_to_fd() */

int send_ack_tcp(BDM_Sync_Message_t * sync_msg, int fd) {

    bionet_asn_buffer_t buf;

    if (bdm_gen_sync_msg_ack_asnbuf(sync_msg, &buf) ) {
        return -1;
    }

    ssize_t bytes = write_data_to_fd(buf.buf, buf.size, &fd);

    if(bytes != buf.size){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            "Failed to send sync message ACK: %s", strerror(errno));
        return -1;
    }

    return 0;
}

int sync_receive_readable_handler(GIOChannel *unused, GIOCondition cond, client_t *client) {
    int bytes_to_read;
    int bytes_read = 0;
    asn_dec_rval_t rval;


    if (cond & (G_IO_ERR | G_IO_HUP | G_IO_NVAL)) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "disconnect event from sync sender");
        disconnect_sync_sender(client);
        return FALSE;
    }

    client->index = 0;

    while (1) {
	bytes_to_read = sizeof(client->buffer) - client->index;
	bytes_read = read(client->fd, &client->buffer[client->index], bytes_to_read);
	if (bytes_read < 0) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error reading from sync sender: %s", strerror(errno));
	    disconnect_sync_sender(client);
	    return FALSE;
	}
	if (bytes_read == 0) {
	    disconnect_sync_sender(client);
	    return FALSE;
	}
	
	client->index += bytes_read;

        rval = ber_decode(NULL, 
			  &asn_DEF_BDM_Sync_Message, 
			  (void **)&client->message.sync_message, 
			  client->buffer, 
			  client->index);
        if (rval.code == RC_OK) {
            if(handle_sync_msg(client->message.sync_message)) {
                return FALSE;
            }
            if(!sync_message_is_ack(client->message.sync_message)){
                send_ack_tcp(client->message.sync_message, client->fd);
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
	    disconnect_sync_sender(client);
            return FALSE;
        } else {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "unknown error with ber_decode (code=%d)", rval.code);
        }

        if (rval.consumed > 0) {
            client->index -= rval.consumed;
            memmove(client->buffer, &client->buffer[rval.consumed], client->index);
        }
    }

    return TRUE;
}

