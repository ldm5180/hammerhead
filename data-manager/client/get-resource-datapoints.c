
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "bdm-client.h"

extern int bdm_fd;


GPtrArray *handle_Resource_Datapoints_Reply(ResourceDatapointsReply_t *rdr) {
    GPtrArray *hab_list;
    int hi;

    hab_list = g_ptr_array_new();

    for (hi = 0; hi < rdr->list.count; hi ++) {
        HardwareAbstractor_t *asn_hab;
        bionet_hab_t *hab;
        int ni;

        asn_hab = rdr->list.array[hi];

        hab = bionet_hab_new((char *)asn_hab->type.buf, (char *)asn_hab->id.buf);
        if (hab == NULL) goto cleanup;

        g_ptr_array_add(hab_list, hab);

        for (ni = 0; ni < asn_hab->nodes.list.count; ni ++) {
            Node_t *asn_node;
            bionet_node_t *node;
            int ri;

            asn_node = asn_hab->nodes.list.array[ni];

            node = bionet_node_new(hab, (char *)asn_node->id.buf);
            if (node == NULL) goto cleanup;

            bionet_hab_add_node(hab, node);

            for (ri = 0; ri < asn_node->resources.list.count; ri ++) {
                Resource_t *asn_resource;
                bionet_resource_data_type_t datatype;
                bionet_resource_flavor_t flavor;
                bionet_resource_t *resource;
                int di;

                asn_resource = asn_node->resources.list.array[ri];

                datatype = bionet_asn_to_datatype(asn_resource->datatype);
                if (datatype == -1) goto cleanup;

                flavor = bionet_asn_to_flavor(asn_resource->flavor);
                if (flavor == -1) goto cleanup;

                resource = bionet_resource_new(node, datatype, flavor, (char *)asn_resource->id.buf);
                if (resource == NULL) goto cleanup;

                bionet_node_add_resource(node, resource);

                for (di = 0; di < asn_resource->datapoints.list.count; di++) {
                    Datapoint_t *asn_datapoint;
                    bionet_datapoint_t *d;

                    asn_datapoint = asn_resource->datapoints.list.array[di];

                    d = bionet_asn_to_datapoint(asn_datapoint, resource);
                    bionet_resource_add_datapoint(resource, d);
                }
            }
        }
    }


    return hab_list;


cleanup:
    // FIXME
    return NULL;
}




GPtrArray *bdm_get_resource_datapoints(const char *resource_name_pattern, struct timeval *start, struct timeval *end) {
    char *hab_type;
    char *hab_id;
    char *node_id;
    char *resource_id;

    BDM_C2S_Message_t m;
    ResourceDatapointsQuery_t *rdpq;

    GPtrArray *retval = NULL;

    int r;
    asn_enc_rval_t enc_rval;


    memset(&m, 0x00, sizeof(BDM_C2S_Message_t));
    m.present = BDM_C2S_Message_PR_resourceDatapointsQuery;
    rdpq = &m.choice.resourceDatapointsQuery;

    r = bionet_split_resource_name(resource_name_pattern, &hab_type, &hab_id, &node_id, &resource_id);
    if (r != 0) {
        goto cleanup0;
    }

    r = OCTET_STRING_fromString(&rdpq->habType, hab_type);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_get_resource_datapoints(): error making OCTET STRING for %s", hab_type);
        goto cleanup0;
    }

    r = OCTET_STRING_fromString(&rdpq->habId, hab_id);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_get_resource_datapoints(): error making OCTET STRING for %s", hab_id);
        goto cleanup1;
    }

    r = OCTET_STRING_fromString(&rdpq->nodeId, node_id);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_get_resource_datapoints(): error making OCTET STRING for %s", node_id);
        goto cleanup2;
    }

    r = OCTET_STRING_fromString(&rdpq->resourceId, resource_id);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_get_resource_datapoints(): error making OCTET STRING for %s", resource_id);
        goto cleanup3;
    }


    r = bionet_timeval_to_GeneralizedTime(start, &rdpq->startTime);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_get_resource_datapoints(): error making GeneralizedTime from %ld.%06ld: %s", (long)start->tv_sec, (long)start->tv_usec, strerror(errno));
        goto cleanup4;
    }

    r = bionet_timeval_to_GeneralizedTime(end, &rdpq->endTime);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_get_resource_datapoints(): error making GeneralizedTime from %ld.%06ld: %s", (long)end->tv_sec, (long)end->tv_usec, strerror(errno));
        goto cleanup4;
    }

    enc_rval = der_encode(&asn_DEF_BDM_C2S_Message, &m, bdm_send_asn, NULL);
    if (enc_rval.encoded == -1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_get_resource_datapoints(): error with der_encode(): %s", strerror(errno));
        goto cleanup4;
    }


    // FIXME: bogus loop to read the reply
    BDM_S2C_Message_t *message = NULL;
    unsigned char buf[10 * 1024];
    int index = 0;
    asn_dec_rval_t dec_rval;
    int total_bytes_read = 0;

    do {
        int r;

        r = read(bdm_fd, &buf[index], sizeof(buf) - index);
        if (r < 0) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_get_resource_datapoints(): error reading reply from server: %s", strerror(errno));
            goto cleanup4;
        }
        if (r == 0) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_get_resource_datapoints(): short read from server: %s", strerror(errno));
            goto cleanup4;
        }

        total_bytes_read += r;
        index += r;

        dec_rval = ber_decode(NULL, &asn_DEF_BDM_S2C_Message, (void **)&message, buf, index);

        if (dec_rval.code == RC_OK) {
            GPtrArray *hab_list;
            if (message->present != BDM_S2C_Message_PR_resourceDatapointsReply) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO, "bdm_get_resource_datapoints(): unexpected message %d", message->present);
                goto cleanup4;
            }
            hab_list = handle_Resource_Datapoints_Reply(&message->choice.resourceDatapointsReply);
            asn_DEF_BDM_S2C_Message.free_struct(&asn_DEF_BDM_S2C_Message, message, 0);
            return hab_list;
        } else if (dec_rval.code == RC_WMORE) {
            // ber_decode is waiting for more data, but so far so good
        } else if (dec_rval.code == RC_FAIL) {
            // received invalid junk
            g_warning("ber_decode failed to decode the server's message");
            return NULL;
        } else {
            g_warning("unknown error with ber_decode (code=%d)", dec_rval.code);
            return NULL;
        }

        if (dec_rval.consumed > 0) {
            index -= dec_rval.consumed;
            if (index > 0) {
                memmove(buf, &buf[dec_rval.consumed], index);
            }
        }
    } while (1);


cleanup4:
    OCTET_STRING_fromString(&rdpq->resourceId, NULL);

cleanup3:
    OCTET_STRING_fromString(&rdpq->nodeId, NULL);

cleanup2:
    OCTET_STRING_fromString(&rdpq->habId, NULL);

cleanup1:
    OCTET_STRING_fromString(&rdpq->habType, NULL);

cleanup0:
    return retval;
}

