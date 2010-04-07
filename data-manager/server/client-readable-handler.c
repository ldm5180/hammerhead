
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>

#include "bionet-asn.h"
#include "bionet-data-manager.h"


int client_readable_handler(GIOChannel *unused, GIOCondition cond, client_t *client) {
    int bytes_to_read;
    int bytes_read;
    asn_dec_rval_t rval;


    if (cond & (G_IO_ERR | G_IO_HUP | G_IO_NVAL)) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "disconnect event from client");
        disconnect_client(client);
        return FALSE;
    }


    bytes_to_read = sizeof(client->buffer) - client->index;
    bytes_read = read(client->fd, &client->buffer[client->index], bytes_to_read);
    if (bytes_read < 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error reading from client: %s", strerror(errno));
        disconnect_client(client);
        return FALSE;
    }
    if (bytes_read == 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "eof from client");
        disconnect_client(client);
        return FALSE;
    }

    client->index += bytes_read;

    do {
        rval = ber_decode(NULL, &asn_DEF_BDM_C2S_Message, (void **)&client->message.C2S_message, client->buffer, client->index);
        if (rval.code == RC_OK) {
            handle_client_message(client, client->message.C2S_message);
            asn_DEF_BDM_C2S_Message.free_struct(&asn_DEF_BDM_C2S_Message, client->message.C2S_message, 0);
            client->message.C2S_message = NULL;
        } else if (rval.code == RC_WMORE) {
            // ber_decode is waiting for more data, but so far so good
        } else if (rval.code == RC_FAIL) {
            // received invalid junk
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "ber_decode failed to decode the client's message");
            disconnect_client(client);
            return FALSE;
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

