
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


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
    GPtrArray *hab_list;
    struct timeval start, end;
    int r;

    BDM_S2C_Message_t reply;
    ResourceDatapointsReply_t *rdpr;
    asn_enc_rval_t asn_r;


    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "    Resource Datapoints Query {");
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "        Resource Name Pattern: %s.%s.%s:%s", rdpq->habType.buf, rdpq->habId.buf, rdpq->nodeId.buf, rdpq->resourceId.buf);
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "        startTime=%s", rdpq->startTime.buf);
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "        endTime=%s", rdpq->endTime.buf);
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "    }");

    r = bionet_GeneralizedTime_to_timeval(&rdpq->startTime, &start);
    if (r != 0) {
        g_log(
            BDM_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "handle_client_message_resourceDatapointsQuery(): error converting GeneralizedTime '%s' to struct timeval: %s",
            rdpq->startTime.buf,
            strerror(errno)
        );
        return;  // FIXME: return an error message to the client
    }

    r = bionet_GeneralizedTime_to_timeval(&rdpq->endTime, &end);
    if (r != 0) {
        g_log(
            BDM_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "handle_client_message_resourceDatapointsQuery(): error converting GeneralizedTime '%s' to struct timeval: %s",
            rdpq->endTime.buf,
            strerror(errno)
        );
        return;  // FIXME: return an error message to the client
    }


    // do that database lookup
    hab_list = db_get_resource_datapoints(rdpq->habType.buf, rdpq->habId.buf, rdpq->nodeId.buf, rdpq->resourceId.buf, &start, &end);


    memset(&reply, 0x00, sizeof(BDM_S2C_Message_t));
    reply.present = BDM_S2C_Message_PR_resourceDatapointsReply;
    rdpr = &reply.choice.resourceDatapointsReply;

    // build the reply message
    // debuggingly print out what we got
    {
        int hi;

        for (hi = 0; hi < hab_list->len; hi ++) {
            int ni;
            HardwareAbstractor_t *asn_hab;
            bionet_hab_t *hab = g_ptr_array_index(hab_list, hi);

            asn_hab = (HardwareAbstractor_t *)calloc(1, sizeof(HardwareAbstractor_t));
            if (asn_hab == NULL) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "handle_client_message_resourceDatapointsQuery(): out of memory!");
                goto cleanup;
            }

            r = asn_sequence_add(&rdpr->list, asn_hab);
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
    // FIXME
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
