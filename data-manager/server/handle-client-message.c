
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "bionet-data-manager.h"
#include "bdm-db.h"
#include "bionet-asn.h"
#include "bdm-asn.h"
#include "cal-server.h"


static int send_message_to_client(const void *buffer, size_t size, void *client_void) {
    client_t *client = client_void;
    return write(client->fd, buffer, size);
}

static int libbdm_process_resourceDatapointsQuery(
        ResourceDatapointsQuery_t *rdpq,
        asn_app_consume_bytes_f *consume_cb, void *consume_data)
{
    GPtrArray *bdm_list;
    struct timeval datapoint_start, datapoint_end;
    int entry_start, entry_end;
    struct timeval *pDatapointStart = NULL;
    struct timeval *pDatapointEnd = NULL;
    int r, bi;
    int ret = -1;

    BDM_S2C_Message_t reply;
    ResourceDatapointsReply_t *rdpr;
    asn_enc_rval_t asn_r;


    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "    Resource Datapoints Query {");
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "        Resource Name Pattern: %s.%s.%s:%s", rdpq->habType.buf, rdpq->habId.buf, rdpq->nodeId.buf, rdpq->resourceId.buf);
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "        datapointStartTime=%s", rdpq->datapointStartTime.buf);
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "        datapointEndTime=%s", rdpq->datapointEndTime.buf);
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "        entryStart=%ld", rdpq->entryStart);
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "        entryEnd=%ld", rdpq->entryEnd);
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "    }");

    r = bionet_GeneralizedTime_to_timeval(&rdpq->datapointStartTime, &datapoint_start);
    if (r != 0) {
        g_log(
            BDM_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "%s(): error converting GeneralizedTime '%s' to struct timeval: %s",
            __FUNCTION__,
            rdpq->datapointStartTime.buf,
            strerror(errno)
        );
        return -1;  // FIXME: return an error message to the client
    }
    if (datapoint_start.tv_sec != 0 || datapoint_start.tv_usec != 0) {
	pDatapointStart = &datapoint_start;
    }

    r = bionet_GeneralizedTime_to_timeval(&rdpq->datapointEndTime, &datapoint_end);
    if (r != 0) {
        g_log(
            BDM_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "%s(): error converting GeneralizedTime '%s' to struct timeval: %s", __FUNCTION__,
            rdpq->datapointEndTime.buf,
            strerror(errno)
        );
        return -1;  // FIXME: return an error message to the client
    }
    if (datapoint_end.tv_sec != 0 || datapoint_end.tv_usec != 0) {
	pDatapointEnd = &datapoint_end;
    }

    entry_start = rdpq->entryStart;

    entry_end = rdpq->entryEnd;
    if (entry_end == -1) {
	entry_end = db_get_latest_entry_seq(main_db);
    }

    // do the database lookup
    bdm_list = db_get_bdmlist(main_db, NULL, 
                                          (const char *)rdpq->habType.buf, 
					  (const char *)rdpq->habId.buf, 
					  (const char *)rdpq->nodeId.buf, 
					  (const char *)rdpq->resourceId.buf, 
                                          NULL, NULL,
					  entry_start, entry_end);
    if (NULL == bdm_list) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
	      "Failed to get a BDM list.");
	return -1;
    }

    memset(&reply, 0x00, sizeof(BDM_S2C_Message_t));
    reply.present = BDM_S2C_Message_PR_resourceDatapointsReply;
    rdpr = &reply.choice.resourceDatapointsReply;

    // build the reply message
    for (bi = 0; bi < bdm_list->len; bi++) {
        DataManager_t * asn_bdm;
	bionet_bdm_t * bdm = g_ptr_array_index(bdm_list, bi);
	if (NULL == bdm) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Failed to get BDM %d from BDM list", bi);
	    goto cleanup;
	}

        //add the BDM to the message
        asn_bdm = bionet_bdm_to_asn(bdm);

        r = asn_sequence_add(&rdpr->bdms.list, asn_bdm);
        if (r != 0) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                  "sync_send_metadata(): error adding BDM to Sync Metadata: %s", strerror(errno));
            goto cleanup;
        }
    } //for (bi = 0; bi < bdm_list->len; bi++)

    //
    // Encode ASN message and send to requesting peer
    //
    asn_r = der_encode(&asn_DEF_BDM_S2C_Message, &reply, consume_cb, consume_data);
    if (asn_r.encoded == -1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "bdm_get_resource_datapoints(): error with der_encode(): %s", strerror(errno));
        goto cleanup;
    }

    ret = 0;


cleanup:
    if(bdm_list){
        bdm_list_free(bdm_list);
    }
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_S2C_Message, &reply);
    return ret;
}

static void handle_client_message_resourceDatapointsQuery(
        client_t *client,
        ResourceDatapointsQuery_t *rdpq) 
{
    libbdm_process_resourceDatapointsQuery(rdpq, send_message_to_client, client);
}

void libbdm_handle_resourceDatapointsQuery(
        const char * peer_name, 
        ResourceDatapointsQuery_t *rdpq) 
{

    bionet_asn_buffer_t buf;
    int r;
    buf.buf = NULL;
    buf.size = 0;

    r = libbdm_process_resourceDatapointsQuery(rdpq, 
            bionet_accumulate_asn_buffer, &buf);

    if ( r < 0 ) goto cleanup;

    // send to the peer that requested the result
    if(!cal_server.sendto(libbdm_cal_handle, peer_name, buf.buf, buf.size)) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "bdm_get_resource_datapoints(): error sending to peer");
        goto cleanup;
    }

    return;

cleanup:
    free(buf.buf);
    buf.buf = NULL;
}



void handle_client_message(client_t *client, BDM_C2S_Message_t *message) {
    switch (message->present) {
        case BDM_C2S_Message_PR_resourceDatapointsQuery: {
            handle_client_message_resourceDatapointsQuery(client, 
                    &message->choice.resourceDatapointsQuery);
            break;
        }

        default: {
            g_message("unknown message choice: %d", message->present);
            break;
        }
    }
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
