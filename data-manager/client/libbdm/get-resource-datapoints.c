
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "libbdm-internal.h"
#include "bdm-util.h"

extern int bdm_fd;

libbdm_datapoint_query_response_t * _libbdm_query_response = NULL;

void bdm_handle_query_response(const cal_event_t *event,
        ResourceDatapointsReply_t *rdr) 
{
    GPtrArray *hab_list;
    int hi;
    int bdm_last_entry = -1;

    hab_list = g_ptr_array_new();


    if (rdr->habs.list.count) {
	bdm_last_entry = rdr->lastEntry;
    }

    for (hi = 0; hi < rdr->habs.list.count; hi ++) {
        HardwareAbstractor_t *asn_hab;
        bionet_hab_t *hab;
        int ni;

        asn_hab = rdr->habs.list.array[hi];

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

            if (bionet_hab_add_node(hab, node)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		      "handle_Resource_Datapoints_Reply(): Failed to add node to hab.");
	    }

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

                if (bionet_node_add_resource(node, resource)) {
		    g_log("", G_LOG_LEVEL_WARNING, 
			  "handle_Resource_Datapoints_Reply(): Failed to add resource to node.");
		    bionet_resource_free(resource);
		    goto cleanup;
		}

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


    libbdm_datapoint_query_response_t * this_response = 
        malloc(sizeof(libbdm_datapoint_query_response_t));
    this_response->hab_list = hab_list;
    this_response->last_entry = bdm_last_entry;

    _libbdm_query_response = this_response;

cleanup:
    return;
}




libbdm_datapoint_query_response_t *bdm_get_resource_datapoints(
    const char * bdm_id,
    const char *resource_name_pattern, 
    struct timeval *datapointStart, 
    struct timeval *datapointEnd,
    int entryStart,
    int entryEnd) 
{
    char *hab_type;
    char *hab_id;
    char *node_id;
    char *resource_id;

    BDM_C2S_Message_t m;
    ResourceDatapointsQuery_t *rdpq;

    bionet_asn_buffer_t buf;
    memset(&buf, 0, sizeof(buf));

    int r;
    asn_enc_rval_t enc_rval;

    struct timeval tv;

    memset(&m, 0x00, sizeof(BDM_C2S_Message_t));
    m.present = BDM_C2S_Message_PR_resourceDatapointsQuery;
    rdpq = &m.choice.resourceDatapointsQuery;

    r = bionet_split_resource_name(resource_name_pattern, &hab_type, &hab_id, &node_id, &resource_id);
    if (r != 0) {
        goto cleanup;
    }

    r = OCTET_STRING_fromString(&rdpq->habType, hab_type);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_get_resource_datapoints(): error making OCTET STRING for %s", hab_type);
        goto cleanup;
    }

    r = OCTET_STRING_fromString(&rdpq->habId, hab_id);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_get_resource_datapoints(): error making OCTET STRING for %s", hab_id);
        goto cleanup;
    }

    r = OCTET_STRING_fromString(&rdpq->nodeId, node_id);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_get_resource_datapoints(): error making OCTET STRING for %s", node_id);
        goto cleanup;
    }

    r = OCTET_STRING_fromString(&rdpq->resourceId, resource_id);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_get_resource_datapoints(): error making OCTET STRING for %s", resource_id);
        goto cleanup;
    }

    if (datapointStart) {
	tv.tv_sec = datapointStart->tv_sec;
	tv.tv_usec = datapointStart->tv_usec;
    } else {
	tv.tv_sec = 0;
	tv.tv_usec = 0;
    }
    r = bionet_timeval_to_GeneralizedTime(&tv, &rdpq->datapointStartTime);
    if (r != 0) {
	if (datapointStart) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bdm_get_resource_datapoints(): error making GeneralizedTime from %ld.%06ld: %m", 
		  (long)datapointStart->tv_sec, (long)datapointStart->tv_usec);
	} else {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bdm_get_resource_datapoints(): error making GeneralizedTime from NULL: %m");
	}
        goto cleanup;
    }

    if (datapointEnd) {
	tv.tv_sec = datapointEnd->tv_sec;
	tv.tv_usec = datapointEnd->tv_usec;
    } else {
	tv.tv_sec = 0;
	tv.tv_usec = 0;
    }
    r = bionet_timeval_to_GeneralizedTime(&tv, &rdpq->datapointEndTime);
    if (r != 0) {
	if (datapointEnd) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bdm_get_resource_datapoints(): error making GeneralizedTime from %ld.%06ld: %m", 
		  (long)datapointEnd->tv_sec, (long)datapointEnd->tv_usec);
	} else {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bdm_get_resource_datapoints(): error making GeneralizedTime from NULL: %m");
	}
        goto cleanup;
    }

    rdpq->entryStart = entryStart;
    rdpq->entryEnd = entryEnd;

    enc_rval = der_encode(&asn_DEF_BDM_C2S_Message, &m, bionet_accumulate_asn_buffer, &buf);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_C2S_Message, &m);
    if (enc_rval.encoded == -1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_get_resource_datapoints(): error with der_encode(): %s", strerror(errno));
        goto cleanup;
    }


    // send the request to the BDM
    // NOTE: cal_client.sendto assumes controll of buf
    cal_client.sendto(bdm_id, buf.buf, buf.size);

    while(_libbdm_query_response == NULL) {
        g_usleep(1000);
        bdm_read();
    }

    libbdm_datapoint_query_response_t * this_response = _libbdm_query_response;
    _libbdm_query_response = NULL;

    return this_response;


cleanup:
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_C2S_Message, &m);

    return NULL;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
