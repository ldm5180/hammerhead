
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "bionet-data-manager.h"
#include "bdm-util.h"


int send_message_to_client(const void *buffer, size_t size, void *client_void) {
    client_t *client = client_void;
    return write(client->fd, buffer, size);
}


static void handle_client_message_resourceDatapointsQuery(client_t *client, ResourceDatapointsQuery_t *rdpq) {
    GPtrArray *bdm_list;
    struct timeval datapoint_start, datapoint_end;
    int entry_start, entry_end;
    struct timeval *pDatapointStart = NULL;
    struct timeval *pDatapointEnd = NULL;
    int r, bi;

    BDM_S2C_Message_t reply;
    ResourceDatapointsReply_t *rdpr;
    asn_enc_rval_t asn_r;


    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "    Resource Datapoints Query {");
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "        Resource Name Pattern: %s.%s.%s:%s", rdpq->habType.buf, rdpq->habId.buf, rdpq->nodeId.buf, rdpq->resourceId.buf);
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "        datapointStartTime=%s", rdpq->datapointStartTime.buf);
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "        datapointEndTime=%s", rdpq->datapointEndTime.buf);
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "        entryStart=%ld", rdpq->entryStart);
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "        entryEnd=%ld", rdpq->entryEnd);
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "    }");

    r = bionet_GeneralizedTime_to_timeval(&rdpq->datapointStartTime, &datapoint_start);
    if (r != 0) {
        g_log(
            BDM_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "handle_client_message_resourceDatapointsQuery(): error converting GeneralizedTime '%s' to struct timeval: %s",
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
            "handle_client_message_resourceDatapointsQuery(): error converting GeneralizedTime '%s' to struct timeval: %s",
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
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "handle_client_message_resourceDatapointsQuery(): out of memory!");
                goto cleanup;
            }

            r = asn_sequence_add(&rdpr->habs.list, asn_hab);
            if (r != 0) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "handle_client_message_resourceDatapointsQuery(): error adding HAB to ResourceDatapointReply: %s", strerror(errno));
                goto cleanup;
            }

            r = OCTET_STRING_fromString(&asn_hab->type, bionet_hab_get_type(hab));
            if (r != 0) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "handle_client_message_resourceDatapointsQuery(): error making OCTET_STRING for HAB-Type %s", bionet_hab_get_type(hab));
                goto cleanup;
            }

            r = OCTET_STRING_fromString(&asn_hab->id, bionet_hab_get_id(hab));
            if (r != 0) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "handle_client_message_resourceDatapointsQuery(): error making OCTET_STRING for HAB-ID %s", bionet_hab_get_id(hab));
                goto cleanup;
            }

            for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni ++) {
                int ri;
                Node_t *asn_node;
                bionet_node_t *node = bionet_hab_get_node_by_index(hab, ni);

                asn_node = (Node_t *)calloc(1, sizeof(Node_t));
                if (asn_node == NULL) {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "handle_client_message_resourceDatapointsQuery(): out of memory!");
                    goto cleanup;
                }

                r = asn_sequence_add(&asn_hab->nodes.list, asn_node);
                if (r != 0) {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "handle_client_message_resourceDatapointsQuery(): error adding Node to ResourceDatapointReply: %s", strerror(errno));
                    goto cleanup;
                }

                r = OCTET_STRING_fromString(&asn_node->id, bionet_node_get_id(node));
                if (r != 0) {
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "handle_client_message_resourceDatapointsQuery(): error making OCTET_STRING for Node-ID %s", bionet_node_get_id(node));
                    goto cleanup;
                }

                for (ri = 0; ri < bionet_node_get_num_resources(node); ri ++) {
                    int di;
                    Resource_t *asn_resource;
                    bionet_resource_t *resource = bionet_node_get_resource_by_index(node, ri);

                    asn_resource = (Resource_t *)calloc(1, sizeof(Resource_t));
                    if (asn_resource == NULL) {
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "handle_client_message_resourceDatapointsQuery(): out of memory!");
                        goto cleanup;
                    }

                    r = asn_sequence_add(&asn_node->resources.list, asn_resource);
                    if (r != 0) {
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "handle_client_message_resourceDatapointsQuery(): error adding Resource to ResourceDatapointReply: %s", strerror(errno));
                        goto cleanup;
                    }

                    r = OCTET_STRING_fromString(&asn_resource->id, bionet_resource_get_id(resource));
                    if (r != 0) {
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "handle_client_message_resourceDatapointsQuery(): error making OCTET_STRING for Resource-ID %s", bionet_resource_get_id(resource));
                        goto cleanup;
                    }

                    asn_resource->flavor = bionet_flavor_to_asn(bionet_resource_get_flavor(resource));
                    if (asn_resource->flavor == -1) {
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "handle_client_message_resourceDatapointsQuery(): resource has invalid flavor");
                        goto cleanup;
                    }

                    asn_resource->datatype = bionet_datatype_to_asn(bionet_resource_get_data_type(resource));
                    if (asn_resource->datatype == -1) {
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "handle_client_message_resourceDatapointsQuery(): resource has invalid datatype");
                        goto cleanup;
                    }

                    for (di = 0; di < bionet_resource_get_num_datapoints(resource); di ++) {
                        bionet_datapoint_t *d = bionet_resource_get_datapoint_by_index(resource, di);
                        Datapoint_t *asn_datapoint;

                        asn_datapoint = bionet_datapoint_to_asn(d);
                        if (asn_datapoint == NULL) {
                            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "handle_client_message_resourceDatapointsQuery(): out of memory!");
                            goto cleanup;
                        }

                        r = asn_sequence_add(&asn_resource->datapoints.list, asn_datapoint);
                        if (r != 0) {
                            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "handle_client_message_resourceDatapointsQuery(): error adding Datapoint to Resource: %s", strerror(errno));
                            goto cleanup;
                        }

                    }
                }
            }
        }
    }

    

    // send the reply to the client
    asn_r = der_encode(&asn_DEF_BDM_S2C_Message, &reply, send_message_to_client, client);
    if (asn_r.encoded == -1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_get_resource_datapoints(): error with der_encode(): %s", strerror(errno));
        goto cleanup;
    }


cleanup:
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_S2C_Message, &reply);
    return;
}


void handle_client_message(client_t *client, BDM_C2S_Message_t *message) {
    switch (message->present) {
        case BDM_C2S_Message_PR_resourceDatapointsQuery: {
            handle_client_message_resourceDatapointsQuery(client, &message->choice.resourceDatapointsQuery);
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
