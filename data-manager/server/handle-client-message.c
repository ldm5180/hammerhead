
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "bionet-data-manager.h"
#include "bionet-asn.h"
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

    // do that database lookup
    bdm_list = db_get_resource_datapoints(main_db,
                                          (const char *)rdpq->habType.buf, 
					  (const char *)rdpq->habId.buf, 
					  (const char *)rdpq->nodeId.buf, 
					  (const char *)rdpq->resourceId.buf, 
					  pDatapointStart, pDatapointEnd, 
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
    // debuggingly print out what we got
    for (bi = 0; bi < bdm_list->len; bi++) {
        int hi;
	bionet_bdm_t * bdm = g_ptr_array_index(bdm_list, bi);

	//BDM-BP TODO someday add the BDM ID to the client message

        rdpr->lastEntry = entry_end;

        for (hi = 0; hi < bionet_bdm_get_num_habs(bdm); hi ++) {
            HardwareAbstractor_t *asn_hab;
            bionet_hab_t *hab = bionet_bdm_get_hab_by_index(bdm, hi);

            asn_hab = (HardwareAbstractor_t *)calloc(1, sizeof(HardwareAbstractor_t));
            if (asn_hab == NULL) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): out of memory!", __FUNCTION__);
                goto cleanup;
            }

            r = bionet_hab_to_asn(hab, asn_hab);
            if (r != 0) {
                free(asn_hab);
                goto cleanup;
            }

            r = asn_sequence_add(&rdpr->habs.list, asn_hab);
            if (r != 0) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error adding HAB to ResourceDatapointReply: %s", __FUNCTION__,  strerror(errno));
                goto cleanup;
            }

        }
    }

    

    //
    // Encode ASN message andsend to requesting peer
    //
    asn_r = der_encode(&asn_DEF_BDM_S2C_Message, &reply, consume_cb, consume_data);
    if (asn_r.encoded == -1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "bdm_get_resource_datapoints(): error with der_encode(): %s", strerror(errno));
        goto cleanup;
    }

    return 0;


cleanup:
    if(bdm_list){
        bdm_list_free(bdm_list);
    }
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_S2C_Message, &reply);
    return -1;
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
    if(!cal_server.sendto(peer_name, buf.buf, buf.size)) {
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
