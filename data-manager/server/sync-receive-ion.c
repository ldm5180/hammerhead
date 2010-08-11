
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

#define _MIN(a,b) (((a) < (b)) ? (a):(b))


static char	*deliveryTypes[] =	{
			"Payload delivered",
			"Reception timed out",
			"Reception interrupted"
					};

/*
static void	handleQuit()
{
	bp_interrupt(client.ion.sap);
}
*/


static BDM_Sync_Message_t * handle_bundle(Sdr sdr, BpDelivery *dlv)
{
    int bytes_to_read;
    int bytes_read;
    unsigned int bundle_bytes_remaining;
    asn_dec_rval_t rval = {0};
    BDM_Sync_Message_t * sync_message = NULL;
    ZcoReader reader;

    bundle_bytes_remaining = (*bdm_bp_funcs.zco_source_data_length)(sdr, dlv->adu);
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Processing a bundle of %d bytes", bundle_bytes_remaining);

    (*bdm_bp_funcs.sdr_begin_xn)(sdr);
    (*bdm_bp_funcs.zco_start_receiving)(sdr, dlv->adu, &reader);

    int buffer_index = 0;
    void * buffer = NULL;
    size_t buffer_size = 0;

    sync_message = NULL;

    while ( bundle_bytes_remaining ) {
        bytes_to_read = buffer_size - buffer_index;
        if(bytes_to_read <= 0)
        {
            buffer_size += 1024;
            buffer = realloc(buffer, buffer_size);
            bytes_to_read = buffer_size - buffer_index;
        }
        bytes_to_read = _MIN(bytes_to_read, bundle_bytes_remaining);

        bytes_read = (*bdm_bp_funcs.zco_receive_source)(sdr, &reader,
                        bytes_to_read, (void*)(buffer+buffer_index));
        if(bytes_read < 0)
        {
            (*bdm_bp_funcs.sdr_cancel_xn)(sdr);
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "ION event: can't receive payload");
            goto fail;
        }

        if(bytes_read != bytes_to_read) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Got %d bytes from zco when %d requested", bytes_read, bytes_to_read);
            goto fail;
        }
        bundle_bytes_remaining -= bytes_read;
        buffer_index += bytes_read;

        rval = ber_decode(NULL, 
                          &asn_DEF_BDM_Sync_Message, 
                          (void **)&sync_message, 
                          buffer, 
                          buffer_index);
        switch ( rval.code ) {
            case RC_OK:
                if (sync_message->present 
                    == BDM_Sync_Message_PR_metadataMessage) 
                {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
                          "sync_receive_ion(): receive Sync Metadata Message");
                    handle_sync_metadata_message(&sync_message->choice.metadataMessage);

                    if(bundle_bytes_remaining > 0) {
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                            "Extra data in bundle after sync metadata content"); 
                    }
                    goto done;
                } else if (sync_message->present 
                    == BDM_Sync_Message_PR_datapointsMessage) 
                {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
                          "sync_receive_ion(): receive Sync Datapoints Message");
                    handle_sync_datapoints_message(&sync_message->choice.datapointsMessage);

                    if(bundle_bytes_remaining > 0) {
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                            "Extra data in bundle after sync datapoints content"); 
                    }
                    goto done;
                } else {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                          "sync_receive_ion(): unknown Sync Message choice");
                }
                goto fail;

            case RC_WMORE:
                // ber_decode is waiting for more data, suck more data
                // from zco
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
    // Ran out of bundle bytes...
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
        "Not enough data in bundle for sync message decode");

fail:
    if(sync_message){
        asn_DEF_BDM_Sync_Message.free_struct(
            &asn_DEF_BDM_Sync_Message,
            sync_message, 0);
        sync_message = NULL;
    }

done:
    (*bdm_bp_funcs.zco_stop_receiving)(sdr, &reader);
    if ((*bdm_bp_funcs.sdr_end_xn)(sdr) < 0)
    {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            "ION event: can't hande delivery");
    }

    free(buffer);

    return sync_message;

}

gpointer dtn_receive_thread(gpointer config) {
    int r;


    int running = 1;

    client_t *client = (client_t*)config;
    
    // One-time setup
    r = (*bdm_bp_funcs.bp_open)(dtn_endpoint_id, &client->ion.sap);
    if (r < 0) {
        if (bdm_bp_funcs.bp_add_endpoint != NULL) {
            if ((*bdm_bp_funcs.bp_add_endpoint)(dtn_endpoint_id, NULL) != 1) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Can't create own endpoint ('%s')", dtn_endpoint_id);
                return NULL;
            }
            r = (*bdm_bp_funcs.bp_open)(dtn_endpoint_id, &client->ion.sap);
        }

        if (r < 0) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Can't open own endpoint ('%s')", dtn_endpoint_id);
            return 0;
        }
    }

    client->ion.sdr = (*bdm_bp_funcs.bp_get_sdr)();

    // Wait for bundles, and dispatch them
    while (!bdm_shutdown_now && running) {
	BpDelivery	dlv;

        if ((*bdm_bp_funcs.bp_receive)(client->ion.sap, &dlv, BP_BLOCKING) < 0)
        {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "bpsink bundle reception failed.");
                running = 0;
                continue;
        }

        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
            "ION event: %s.\n", deliveryTypes[dlv.result - 1]);

        if (dlv.result == BpPayloadPresent) {
            client->message.sync_message = handle_bundle(client->ion.sdr, &dlv);
            if(NULL == client->message.sync_message) {
                running = 0;
            }
        }

        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
            "ION bundle processed: %d. Releasing delivery", running);
                
        

        (*bdm_bp_funcs.bp_release_delivery)(&dlv, 1);
    }

    // Shutdown
    (*bdm_bp_funcs.bp_close)(client->ion.sap);
    (*bdm_bp_funcs.writeErrmsgMemos)();

    return NULL;
}

#endif // ENABLE_ION
