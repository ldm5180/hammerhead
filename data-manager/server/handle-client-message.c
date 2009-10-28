
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "bionet-data-manager.h"
#include "bdm-util.h"
#include "bionet-asn.h"
#include "cal-server.h"


void libbdm_handle_resourceDatapointsQuery(
        const char * peer_name, 
        ResourceDatapointsQuery_t *rdpq) 
{
    GPtrArray *bdm_list;
    struct timeval datapoint_start, datapoint_end;
    int entry_start, entry_end;
    struct timeval *pDatapointStart = NULL;
    struct timeval *pDatapointEnd = NULL;
    int r, bi;

    bionet_asn_buffer_t buf;

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
        return;  // FIXME: return an error message to the client
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
        return;  // FIXME: return an error message to the client
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
	return;
    }

    memset(&reply, 0x00, sizeof(BDM_S2C_Message_t));
    reply.present = BDM_S2C_Message_PR_resourceDatapointsReply;
    rdpr = &reply.choice.resourceDatapointsReply;

    // build the reply message
    // debuggingly print out what we got
    for (bi = 0; bi < bdm_list->len; bi++) {
        int hi;
	bdm_t * bdm = g_ptr_array_index(bdm_list, bi);
	GPtrArray * hab_list = bdm->hab_list;

	//BDM-BP TODO someday add the BDM ID to the client message

        rdpr->lastEntry = entry_end;

        for (hi = 0; hi < hab_list->len; hi ++) {
            int ni;
            HardwareAbstractor_t *asn_hab;
            bionet_hab_t *hab = g_ptr_array_index(hab_list, hi);

            asn_hab = (HardwareAbstractor_t *)calloc(1, sizeof(HardwareAbstractor_t));
            if (asn_hab == NULL) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): out of memory!", __FUNCTION__);
                goto cleanup;
            }

            r = asn_sequence_add(&rdpr->habs.list, asn_hab);
            if (r != 0) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error adding HAB to ResourceDatapointReply: %s", __FUNCTION__,  strerror(errno));
                goto cleanup;
            }

            r = OCTET_STRING_fromString(&asn_hab->type, bionet_hab_get_type(hab));
            if (r != 0) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error making OCTET_STRING for HAB-Type %s", __FUNCTION__, bionet_hab_get_type(hab));
                goto cleanup;
            }

            r = OCTET_STRING_fromString(&asn_hab->id, bionet_hab_get_id(hab));
            if (r != 0) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error making OCTET_STRING for HAB-ID %s", __FUNCTION__, bionet_hab_get_id(hab));
                goto cleanup;
            }

            for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni ++) {
                int ri;
                Node_t *asn_node;
                bionet_node_t *node = bionet_hab_get_node_by_index(hab, ni);

                asn_node = (Node_t *)calloc(1, sizeof(Node_t));
                if (asn_node == NULL) {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): out of memory!", __FUNCTION__);
                    goto cleanup;
                }

                r = asn_sequence_add(&asn_hab->nodes.list, asn_node);
                if (r != 0) {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error adding Node to ResourceDatapointReply: %s", __FUNCTION__, strerror(errno));
                    goto cleanup;
                }

                r = OCTET_STRING_fromString(&asn_node->id, bionet_node_get_id(node));
                if (r != 0) {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error making OCTET_STRING for Node-ID %s", __FUNCTION__, bionet_node_get_id(node));
                    goto cleanup;
                }

                for (ri = 0; ri < bionet_node_get_num_resources(node); ri ++) {
                    int di;
                    Resource_t *asn_resource;
                    bionet_resource_t *resource = bionet_node_get_resource_by_index(node, ri);

                    asn_resource = (Resource_t *)calloc(1, sizeof(Resource_t));
                    if (asn_resource == NULL) {
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): out of memory!", __FUNCTION__);
                        goto cleanup;
                    }

                    r = asn_sequence_add(&asn_node->resources.list, asn_resource);
                    if (r != 0) {
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error adding Resource to ResourceDatapointReply: %s", __FUNCTION__, strerror(errno));
                        goto cleanup;
                    }

                    r = OCTET_STRING_fromString(&asn_resource->id, bionet_resource_get_id(resource));
                    if (r != 0) {
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error making OCTET_STRING for Resource-ID %s", __FUNCTION__, bionet_resource_get_id(resource));
                        goto cleanup;
                    }

                    asn_resource->flavor = bionet_flavor_to_asn(bionet_resource_get_flavor(resource));
                    if (asn_resource->flavor == -1) {
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): resource has invalid flavor", __FUNCTION__);
                        goto cleanup;
                    }

                    asn_resource->datatype = bionet_datatype_to_asn(bionet_resource_get_data_type(resource));
                    if (asn_resource->datatype == -1) {
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): resource has invalid datatype", __FUNCTION__);
                        goto cleanup;
                    }

                    for (di = 0; di < bionet_resource_get_num_datapoints(resource); di ++) {
                        bionet_datapoint_t *d = bionet_resource_get_datapoint_by_index(resource, di);
                        Datapoint_t *asn_datapoint;

                        asn_datapoint = bionet_datapoint_to_asn(d);
                        if (asn_datapoint == NULL) {
                            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): out of memory!", __FUNCTION__);
                            goto cleanup;
                        }

                        r = asn_sequence_add(&asn_resource->datapoints.list, asn_datapoint);
                        if (r != 0) {
                            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error adding Datapoint to Resource: %s", __FUNCTION__, strerror(errno));
                            goto cleanup;
                        }

                    }
                }
            }
        }
    }

    

    //
    // Encode ASN message andsend to requesting peer
    //
    buf.buf = NULL;
    buf.size = 0;
    asn_r = der_encode(&asn_DEF_BDM_S2C_Message, &reply, bionet_accumulate_asn_buffer, &buf);
    if (asn_r.encoded == -1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "bdm_get_resource_datapoints(): error with der_encode(): %s", strerror(errno));
        free(buf.buf);
        buf.buf = NULL;
        goto cleanup;
    }

    // send to the peer that requested the result
    if(!cal_server.sendto(peer_name, buf.buf, buf.size)) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "bdm_get_resource_datapoints(): error sending to peer");
        free(buf.buf);
        buf.buf = NULL;
        goto cleanup;
    }
    buf.buf = NULL;


cleanup:
    if(bdm_list){
        bdm_list_free(bdm_list);
    }
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_S2C_Message, &reply);
    return;
}


void handle_client_message(client_t *client, BDM_C2S_Message_t *message) {
    switch (message->present) {
        case BDM_C2S_Message_PR_resourceDatapointsQuery: {
            /* TODO: Are direct client connections needed with 'force subscribe?'
            libbdm_handle_resourceDatapointsQuery(client->peer_name,
                    &message->choice.resourceDatapointsQuery);
            */
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
