
// Copyright (c) 2008-2009, Regents of the University of Colorado.
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
#include "bdm-util.h"



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

gpointer dtn_receive_thread(gpointer config) {


    int running = 1;

    client_t *client = (client_t*)config;
    
    // One-time setup
    if (bp_open(dtn_endpoint_id, &client->ion.sap) < 0)
    {
#ifdef HAVE_BP_ADD_ENDPOINT
        if(bp_add_endpoint(dtn_endpoint_id, NULL) != 1) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "Can't create own endpoint ('%s')", dtn_endpoint_id);
            return NULL;
        } else if(bp_open(dtn_endpoint_id, &client->ion.sap) < 0)
#endif
        {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "Can't open own endpoint ('%s')", dtn_endpoint_id);
            return 0;
        }
    }

    client->ion.sdr = bp_get_sdr();

    // Wait for bundles, and dispatch them
    while (!bdm_shutdown_now && running) {
	BpDelivery	dlv;

        if (bp_receive(client->ion.sap, &dlv, BP_BLOCKING) < 0)
        {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "bpsink bundle reception failed.");
                running = 0;
                continue;
        }

        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
            "ION event: %s.\n", deliveryTypes[dlv.result - 1]);

        if (dlv.result == BpPayloadPresent)
        {
            int bytes_to_read;
            int bytes_read;
            asn_dec_rval_t rval = {0};

            //contentLength = zco_source_data_length(client->ion.sdr, dlv.adu);
            sdr_begin_xn(client->ion.sdr);
            zco_start_receiving(client->ion.sdr, dlv.adu, &client->ion.reader);

            client->index = 0;

            do {

	        bytes_to_read = sizeof(client->buffer) - client->index;
                bytes_read = zco_receive_source(client->ion.sdr, &client->ion.reader,
                                bytes_to_read, (void*)(client->buffer+client->index));
                if(bytes_read < 0)
                {
                    sdr_cancel_xn(client->ion.sdr);
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                        "ION event: can't receive payload");
                    running = 0;
                    continue;
                }
                
                client->index += bytes_read;

                rval = ber_decode(NULL, 
                                  &asn_DEF_BDM_Sync_Message, 
                                  (void **)&client->message.sync_message, 
                                  client->buffer, 
                                  client->index);
                switch ( rval.code ) {
                    case RC_OK:
                        if (client->message.sync_message->present 
                            == BDM_Sync_Message_PR_metadataMessage) 
                        {
                            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
                                  "sync_receive_ion(): receive Sync Metadata Message");
                            handle_sync_metadata_message(client, 
                                &client->message.sync_message->choice.metadataMessage);
                        } else if (client->message.sync_message->present 
                            == BDM_Sync_Message_PR_datapointsMessage) 
                        {
                            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
                                  "sync_receive_ion(): receive Sync Datapoints Message");
                            handle_sync_datapoints_message(client, 
                                &client->message.sync_message->choice.datapointsMessage);
                        } else {
                            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                                  "sync_receive_ion(): unknown Sync Message choice");
                        }
                        asn_DEF_BDM_Sync_Message.free_struct(
                            &asn_DEF_BDM_Sync_Message,
                            client->message.sync_message, 0);
                        client->message.sync_message = NULL;
                        break;

                    case RC_WMORE:
                        // ber_decode is waiting for more data, suck more data
                        // from zco
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
                            "ber_decode: bundle too small");
                        break;

                    case RC_FAIL:
                        // received invalid junk
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                            "ber_decode failed to decode the sync sender's message");
                        break;

                    default:
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                            "unknown error with ber_decode (code=%d)", rval.code);
                        break;

                }

                if (rval.consumed > 0) {
                    client->index -= rval.consumed;
                    memmove(client->buffer, client->buffer + rval.consumed, client->index);
                }
            } while (running && (rval.consumed > 0) && (client->index > 0));

            zco_stop_receiving(client->ion.sdr, &client->ion.reader);
            if (sdr_end_xn(client->ion.sdr) < 0)
            {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                    "ION event: can't hande delivery");
                running = 0;
                continue;
            }
        }

        bp_release_delivery(&dlv, 1);
    }

    // Shutdown
    bp_close(client->ion.sap);
    writeErrmsgMemos();

    return NULL;
}

#endif // ENABLE_ION
