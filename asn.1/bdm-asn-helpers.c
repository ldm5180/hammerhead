
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <glib.h>

#include "bionet-asn.h"
#include "bionet-util.h"

//#include "bionet-data-manager.h"
#include "bionet-util.h"
#include "util/protected.h"

#include "BDMResourceMetadata.h"
#include "BDMResourceDatapoints.h"
#include "BDM-S2C-Message.h"
#include "BDM-Resource.h"
#include "BDM-Datapoint.h"

int bionet_event_to_asn_r(bionet_event_t * event, BDM_Event_t * asn_event) {
    bionet_timeval_to_GeneralizedTime(bionet_event_get_timestamp(event), &asn_event->timestamp);

    asn_event->entrySeq = bionet_event_get_seq(event);

    bionet_event_type_t type = bionet_event_get_type(event);
    asn_event->type = (type==BIONET_EVENT_LOST)?BDM_Event_Type_lost:BDM_Event_Type_new;

    {
        char * time_str = bionet_event_get_timestamp_as_str(event);
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
            "           %c %s",
                (type==BIONET_EVENT_LOST)?'-':'+', time_str);
        free(time_str);
    }

    return 0;

}

typedef A_SEQUENCE_OF(struct BDM_Event) asn_event_list_t;
static int _add_event_to_list(
        asn_event_list_t *list,
        bionet_event_t * event)
{
    int r = 0;
    BDM_Event_t * asn_event = calloc(1, sizeof(BDM_Event_t));
    if(NULL == asn_event) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Out of memory");
        return -1;
    }

    r = bionet_event_to_asn_r(event, asn_event);
    if(r) return -1;

    r = asn_sequence_add(list, asn_event);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
              "bdm_to_asn(): error adding BDM to ASN Metadata: %s", strerror(errno));
        free(asn_event);
        return -1;
    }

    return 0;
}

static BDM_Resource_t *bionet_resource_to_bdm_asn(bionet_resource_t *resource) {
    BDM_Resource_t *asn_resource;
    int r;
    int di, ei;

    asn_resource = (BDM_Resource_t *)calloc(1, sizeof(BDM_Resource_t));
    if (asn_resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): out of memory!", __FUNCTION__);
        return NULL;
    }

    r = OCTET_STRING_fromString(&asn_resource->id, bionet_resource_get_id(resource));
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "%s(): error making OCTET_STRING for Resource-ID %s", __FUNCTION__,
	      bionet_resource_get_id(resource));
        goto cleanup;
    }

    asn_resource->flavor = bionet_flavor_to_asn(bionet_resource_get_flavor(resource));
    if (asn_resource->flavor == -1) {
      g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	    "%s(): invalid resource flavor %d for Resource %s", __FUNCTION__, 
	    bionet_resource_get_flavor(resource), bionet_resource_get_id(resource));
        goto cleanup;
    }

    asn_resource->datatype = bionet_datatype_to_asn(bionet_resource_get_data_type(resource));
    if (asn_resource->datatype == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "%s(): invalid Resource Datatype %d for Resource %s", __FUNCTION__, 
	      bionet_resource_get_data_type(resource), bionet_resource_get_id(resource));
        goto cleanup;
    }

    for (di = 0; di < bionet_resource_get_num_datapoints(resource); di ++) {
        bionet_datapoint_t *d = bionet_resource_get_datapoint_by_index(resource, di);

        for (ei = 0; ei < bionet_datapoint_get_num_events(d); ei++) {
            BDM_Datapoint_t *asn_datapoint = calloc(1, sizeof(BDM_Datapoint_t));
            if(NULL == asn_datapoint) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Out of memory");
                goto cleanup;
            }

            r = asn_sequence_add(&asn_resource->datapoints.list, asn_datapoint);
            if (r != 0) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error adding Datapoint to Resource: %s", __FUNCTION__, strerror(errno));
                goto cleanup;
            }

            if(NULL == bionet_datapoint_to_asn_r(d, &asn_datapoint->datapoint)) {
                goto cleanup;
            }

            r = bionet_event_to_asn_r(bionet_datapoint_get_event_by_index(d, ei), &asn_datapoint->event);
            if (r != 0) goto cleanup;
        }

    }

    return asn_resource;


cleanup:
    ASN_STRUCT_FREE(asn_DEF_BDM_Resource, asn_resource);
    return NULL;
}

BDM_Node_t * bionet_node_to_bdm_asn(const bionet_node_t *node) {
    int r;
    int ri;
    int ei;

    BDM_Node_t * asn_node;

    asn_node = (BDM_Node_t *)calloc(1, sizeof(BDM_Node_t));
    if (asn_node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): out of memory!", __FUNCTION__);
        return NULL;
    }

    r = OCTET_STRING_fromString(&asn_node->id, bionet_node_get_id(node));
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
              "%s(): error making OCTET_STRING for Node-ID %s", __FUNCTION__, bionet_node_get_id(node));
        goto cleanup;
    }

    r = OCTET_STRING_fromBuf(&asn_node->uid,
            (const char *)bionet_node_get_uid(node), 
            BDM_UUID_LEN);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
              "%s(): error making OCTET_STRING for Node-ID %s", __FUNCTION__, bionet_node_get_id(node));
        goto cleanup;
    }

    // Add new/lost Node events
    for (ei = 0; ei < bionet_node_get_num_events(node); ei++) {
        _add_event_to_list((asn_event_list_t *)&asn_node->events.list, 
                bionet_node_get_event_by_index(node, ei));
    }

    // 
    // the Node's Resources
    //

    for (ri = 0; ri < bionet_node_get_num_resources(node); ri ++) {
        BDM_Resource_t *asn_resource;
        bionet_resource_t *resource = bionet_node_get_resource_by_index(node, ri);

        asn_resource = bionet_resource_to_bdm_asn(resource);
        if (asn_resource == NULL) goto cleanup;

        r = asn_sequence_add(&asn_node->resources.list, asn_resource);
        if (r != 0) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_to_asn(): error adding ASN.1 Resource to ASN.1 Node structure: %s", strerror(errno));
            ASN_STRUCT_FREE(asn_DEF_BDM_Resource, asn_resource);
            goto cleanup;
        }
    }


    // 
    // the Node's Streams
    //

    /* TODO: Support streams in BDM
    for (i = 0; i < bionet_node_get_num_streams(node); i ++) {
        Stream_t *asn_stream;
        bionet_stream_t *stream = bionet_node_get_stream_by_index(node, i);

        asn_stream = bionet_stream_to_asn(stream);
        if (asn_stream == NULL) {
            // an error has been logged already
            goto cleanup;
        }

        r = asn_sequence_add(&asn_node->streams.list, asn_stream);
        if (r != 0) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_to_asn(): error adding ASN.1 Stream to ASN.1 Node structure: %s", strerror(errno));
            ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_Stream, asn_stream);
            goto cleanup;
        }
    }
    */


    return asn_node;


cleanup:
    ASN_STRUCT_FREE(asn_DEF_BDM_Node, asn_node);
    return NULL;
}

int bionet_hab_to_bdm_asn(const bionet_hab_t * hab, BDM_HardwareAbstractor_t * asn_hab) {
    int r, ni, ei;

    memset(asn_hab, 0x00, sizeof(BDM_HardwareAbstractor_t));

    r = OCTET_STRING_fromString(&asn_hab->type, bionet_hab_get_type(hab));
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error making OCTET_STRING for HAB-Type %s", __FUNCTION__, bionet_hab_get_type(hab));
        goto cleanup;
    }

    r = OCTET_STRING_fromString(&asn_hab->id, bionet_hab_get_id(hab));
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error making OCTET_STRING for HAB-ID %s", __FUNCTION__, bionet_hab_get_id(hab));
        goto cleanup;
    }

    // Add new/lost hab events
    for (ei = 0; ei < bionet_hab_get_num_events(hab); ei++) {
        _add_event_to_list((asn_event_list_t *)&asn_hab->events.list, 
                bionet_hab_get_event_by_index(hab, ei));
    }

    for (ni = 0; ni < bionet_hab_get_num_nodes(hab); ni ++) {
        BDM_Node_t *asn_node;
        bionet_node_t *node = bionet_hab_get_node_by_index(hab, ni);
        if (NULL == node) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
                  "%s(): Failed to get node %d from HAB %s", __FUNCTION__, ni, bionet_hab_get_name(hab));
        }

        asn_node = bionet_node_to_bdm_asn(node);
        if (NULL == asn_node) goto cleanup;

        r = asn_sequence_add(&asn_hab->nodes.list, asn_node);
        if (r != 0) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error adding Node to ResourceDatapointReply: %s", __FUNCTION__, strerror(errno));
            return -1;
        }
    }

    return 0;

cleanup:
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_HardwareAbstractor, asn_hab);
    return -1;
}

/* Populate the new/lost hab structure.
 * on error, the caller must cleanup the buffer */
static int bdm_new_lost_hab_to_asn_r(
        const char * hab_type, 
        const char * hab_id,
        long entry_seq, 
        const struct timeval *timestamp, 
        const char * bdm_id,
        BDMNewLostHab_t *nh)
{
    int r;
    r = OCTET_STRING_fromBuf(&nh->habId, hab_id, -1);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set hab id", __FUNCTION__);
        return -1;
    }

    r = OCTET_STRING_fromBuf(&nh->habType, hab_type, -1);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set hab type", __FUNCTION__);
        return -1;
    }

    r = OCTET_STRING_fromBuf(&nh->bdmId, bdm_id, -1);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set bdm id", __FUNCTION__);
        return -1;
    }

    r = bionet_timeval_to_GeneralizedTime(timestamp, &nh->timestamp);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error setting timestamp of Event: %s", 
                __FUNCTION__, strerror(errno));
        return -1;
    }

    nh->entrySeq = entry_seq;

    return 0;
}

int bdm_new_hab_to_asnbuf(
        const char * hab_type, 
        const char * hab_id,
        long entry_seq, 
        const struct timeval *timestamp, 
        const char * bdm_id,
        bionet_asn_buffer_t *buf)
{
    BDM_S2C_Message_t m;
    BDMNewLostHab_t *nh;
    asn_enc_rval_t asn_r;

    int r;
    buf->buf = NULL;
    buf->size = 0;

    memset(&m, 0x00, sizeof(m));
    m.present = BDM_S2C_Message_PR_newHab;
    nh = &m.choice.newHab;

    r = bdm_new_lost_hab_to_asn_r(hab_type, hab_id, entry_seq, timestamp, bdm_id, nh);
    if( r ) {
        goto cleanup;
    }

    // 
    // serialize the S2C-Message
    //

    asn_r = der_encode(&asn_DEF_BDM_S2C_Message, &m, bionet_accumulate_asn_buffer, buf);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_S2C_Message, &m);
    if (asn_r.encoded == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error with der_encode(): %s", 
                __FUNCTION__, strerror(errno));
        if (buf->buf != NULL) {
            free(buf->buf);
            buf->buf = NULL;
        }
        goto cleanup;
    }

    return 0;


cleanup:
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_S2C_Message, &m);
    return -1;
}

int bdm_lost_hab_to_asnbuf(
        const char * hab_type, 
        const char * hab_id,
        long entry_seq, 
        const struct timeval *timestamp, 
        const char * bdm_id,
        bionet_asn_buffer_t *buf)
{
    BDM_S2C_Message_t m;
    BDMNewLostHab_t *lh;
    asn_enc_rval_t asn_r;

    int r;
    buf->buf = NULL;
    buf->size = 0;

    memset(&m, 0x00, sizeof(m));
    m.present = BDM_S2C_Message_PR_lostHab;
    lh = &m.choice.lostHab;

    r = bdm_new_lost_hab_to_asn_r(hab_type, hab_id, entry_seq, timestamp, bdm_id, lh);
    if( r ) {
        goto cleanup;
    }

    // 
    // serialize the S2C-Message
    //

    asn_r = der_encode(&asn_DEF_BDM_S2C_Message, &m, bionet_accumulate_asn_buffer, buf);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_S2C_Message, &m);
    if (asn_r.encoded == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error with der_encode(): %s", 
                __FUNCTION__, strerror(errno));
        if (buf->buf != NULL) {
            free(buf->buf);
            buf->buf = NULL;
        }
        goto cleanup;
    }

    return 0;


cleanup:
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_S2C_Message, &m);
    return -1;
}

int bdm_new_lost_node_to_asn_r(
        bionet_node_t *node,
        bionet_event_t * event,
        long entry_seq,
        BDMNewLostNode_t *nn) 
{
    bionet_hab_t * hab;

    int r;
    hab = bionet_node_get_hab(node);

    r = OCTET_STRING_fromBuf(&nn->bdmId, bionet_event_get_bdm_id(event), -1);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set bdm id", __FUNCTION__);
        return -1;
    }

    r = bionet_timeval_to_GeneralizedTime(bionet_event_get_timestamp(event), &nn->timestamp);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error setting timestamp: %s", 
                __FUNCTION__, strerror(errno));
        return -1;
    }

    r = OCTET_STRING_fromBuf(&nn->habType, bionet_hab_get_type(hab), -1);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set hab type", __FUNCTION__);
        return -1;
    }

    r = OCTET_STRING_fromBuf(&nn->habId, bionet_hab_get_id(hab), -1);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set hab id", __FUNCTION__);
        return -1;
    }

    r = OCTET_STRING_fromBuf(&nn->uid, (const char *)bionet_node_get_uid(node), BDM_UUID_LEN);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set hab id", __FUNCTION__);
        return -1;
    }


    nn->entrySeq = entry_seq;

    bionet_node_to_asn(node, &nn->node);

    return 0;
}

int bdm_new_node_to_asnbuf(
        bionet_node_t *node,
        bionet_event_t * event,
        long entry_seq,
        bionet_asn_buffer_t *buf) 
{
    BDM_S2C_Message_t m;
    BDMNewLostNode_t *nn;
    asn_enc_rval_t asn_r;

    int r;
    buf->buf = NULL;
    buf->size = 0;

    memset(&m, 0x00, sizeof(m));
    m.present = BDM_S2C_Message_PR_newNode;
    nn = &m.choice.newNode;

    r = bdm_new_lost_node_to_asn_r(node, event, entry_seq, nn);
    if ( r ) {
        goto cleanup;
    }


    // 
    // serialize the S2C-Message
    //

    asn_r = der_encode(&asn_DEF_BDM_S2C_Message, &m, bionet_accumulate_asn_buffer, buf);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_S2C_Message, &m);
    if (asn_r.encoded == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error with der_encode(): %s", 
                __FUNCTION__, strerror(errno));
        if (buf->buf != NULL) {
            free(buf->buf);
            buf->buf = NULL;
        }
        goto cleanup;
    }

    return 0;


cleanup:
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_S2C_Message, &m);
    return -1;
}

int bdm_lost_node_to_asnbuf(
        bionet_node_t * node,
        bionet_event_t * event,
        long entry_seq, 
        bionet_asn_buffer_t *buf)
{
    BDM_S2C_Message_t m;
    BDMNewLostNode_t *ln;
    asn_enc_rval_t asn_r;

    int r;
    buf->buf = NULL;
    buf->size = 0;

    memset(&m, 0x00, sizeof(m));
    m.present = BDM_S2C_Message_PR_lostNode;
    ln = &m.choice.lostNode;

    r = bdm_new_lost_node_to_asn_r(node, event, entry_seq, ln);
    if ( r ) {
        goto cleanup;
    }

    // 
    // serialize the S2C-Message
    //

    asn_r = der_encode(&asn_DEF_BDM_S2C_Message, &m, bionet_accumulate_asn_buffer, buf);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_S2C_Message, &m);
    if (asn_r.encoded == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error with der_encode(): %s", 
                __FUNCTION__, strerror(errno));
        if (buf->buf != NULL) {
            free(buf->buf);
            buf->buf = NULL;
        }
        goto cleanup;
    }

    return 0;


cleanup:
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_S2C_Message, &m);
    return -1;
}

//
// Send the metadata for each resource when a new node arrives, in case
// there is a resource subscription, but no node subscription
//
int bdm_resource_metadata_to_asnbuf(
        bionet_resource_t *resource,
        long entry_seq,
        bionet_asn_buffer_t *buf) 
{
    BDM_S2C_Message_t m;
    BDMResourceMetadata_t *rm;
    asn_enc_rval_t asn_r;
    bionet_hab_t * hab;
    bionet_node_t * node;

    int r;
    buf->buf = NULL;
    buf->size = 0;

    node = bionet_resource_get_node(resource);
    hab = bionet_node_get_hab(node);

    memset(&m, 0x00, sizeof(m));
    m.present = BDM_S2C_Message_PR_resourceMetadata;
    rm = &m.choice.resourceMetadata;

    r = OCTET_STRING_fromBuf(&rm->habType, bionet_hab_get_type(hab), -1);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set hab type", __FUNCTION__);
        goto cleanup;
    }

    r = OCTET_STRING_fromBuf(&rm->habId, bionet_hab_get_id(hab), -1);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set hab id", __FUNCTION__);
        goto cleanup;
    }

    r = OCTET_STRING_fromBuf(&rm->nodeId, bionet_node_get_id(node), -1);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set node id", __FUNCTION__);
        goto cleanup;
    }

    r = OCTET_STRING_fromBuf(&rm->nodeUid, (const char *)bionet_node_get_uid(node), BDM_UUID_LEN);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set node uid", __FUNCTION__);
        goto cleanup;
    }

    r = OCTET_STRING_fromBuf(&rm->resourceId, 
            bionet_resource_get_id(resource), -1);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set resource key", __FUNCTION__);
        goto cleanup;
    }

    rm->flavor = bionet_flavor_to_asn(bionet_resource_get_flavor(resource));
    if (rm->flavor == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "%s(): error making ResourceFlavor from Resource Flavor %d", 
              __FUNCTION__,
	      bionet_resource_get_flavor(resource));
        goto cleanup;
    }

    // resource data type
    rm->datatype = bionet_datatype_to_asn(bionet_resource_get_data_type(resource));
    if (rm->datatype == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "%s(): error making ResourceDatatype from Resource Datatype %d", 
              __FUNCTION__,
	      bionet_resource_get_data_type(resource));
        goto cleanup;
    }

    // 
    // serialize the S2C-Message
    //

    asn_r = der_encode(&asn_DEF_BDM_S2C_Message, &m, bionet_accumulate_asn_buffer, buf);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_S2C_Message, &m);
    if (asn_r.encoded == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error with der_encode(): %s", 
                __FUNCTION__, strerror(errno));
        if (buf->buf != NULL) {
            free(buf->buf);
            buf->buf = NULL;
        }
        goto cleanup;
    }

    return 0;


cleanup:
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_S2C_Message, &m);
    return -1;
}


int bdm_resource_datapoints_to_asnbuf(
        bionet_resource_t *resource,
        long entry_seq,
        bionet_asn_buffer_t *buf)
{
    BDM_S2C_Message_t m;
    BDMResourceDatapoints_t *rd;
    asn_enc_rval_t asn_r;
    int r;
    int di;
    bionet_hab_t * hab;
    bionet_node_t * node;
    BDMPublishDatapoint_t * asn_pd = NULL;


    buf->buf = NULL;
    buf->size = 0;

    node = bionet_resource_get_node(resource);
    if(NULL == node) g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "%s(): Internal error", __FUNCTION__);

    hab = bionet_node_get_hab(node);
    if(NULL == hab) g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "%s(): Internal error", __FUNCTION__);

    memset(&m, 0x00, sizeof(m));
    m.present = BDM_S2C_Message_PR_datapointsUpdate;
    rd = &m.choice.datapointsUpdate;

    r = OCTET_STRING_fromBuf(&rd->nodeUid, (const char *)bionet_node_get_uid(node), BDM_UUID_LEN);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set node uid", __FUNCTION__);
        goto cleanup;
    }

    r = OCTET_STRING_fromBuf(&rd->resourceId, 
            bionet_resource_get_id(resource), -1);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set resource key", __FUNCTION__);
        goto cleanup;
    }

    //walk list of datapoints and add each one to the message
    for (di = 0; di < bionet_resource_get_num_datapoints(resource); di++) {
        int ei;
        bionet_datapoint_t * d = bionet_resource_get_datapoint_by_index(resource, di);
        if (NULL == d) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
                  "%s(): Failed to get datapoint %d from Resource %s", __FUNCTION__,
                  di, bionet_resource_get_name(resource));
        }

        if(bionet_datapoint_get_num_events(d) < 1) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
                  "%s(): Failed to find events for datapoint %d from Resource %s", __FUNCTION__,
                  di, bionet_resource_get_name(resource));
            continue;
        }

        // TODO: Multiple events could be grouped in the BDMPublishdatapoint message,
        // but I'm guessing it unlikely there will ever be more than one event per datapoint
        // when publishing.
        //
        // Instead, if that ever happens, we'll generate a duplicate datapoint for each duplicate event
        for (ei = 0; ei < bionet_datapoint_get_num_events(d); ei++) {
            asn_pd = calloc(1, sizeof(BDMPublishDatapoint_t));
            if(NULL == asn_pd) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                      "%s(): out of memory!", __FUNCTION__);
                goto cleanup;
            }

            bionet_event_t * e = bionet_datapoint_get_event_by_index(d, ei);
            if (NULL == e) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
                      "%s(): Failed to get event %d from Datapoint", __FUNCTION__, ei);
                goto cleanup;
            }
            
            if(NULL == bionet_datapoint_to_asn_r(d, &asn_pd->datapoint)) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                      "%s(): out of memory!", __FUNCTION__);
                goto cleanup;
            }

            r = bionet_timeval_to_GeneralizedTime(bionet_event_get_timestamp(e), &asn_pd->timestamp);
            if (r != 0) {
                g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error setting timestamp: %s", 
                        __FUNCTION__, strerror(errno));
                goto cleanup;
            }

            asn_pd->entrySeq = entry_seq;

            r = OCTET_STRING_fromBuf(&asn_pd->bdmId, bionet_event_get_bdm_id(e), -1);
            if (r != 0) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                      "%s(): Failed to set bdm-id", __FUNCTION__);
                goto cleanup;
            }
            
            r = asn_sequence_add(&rd->newDatapointsBDM.list, asn_pd);
            if (r != 0) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                      "%s(): error adding Datapoint to Resource: %m", __FUNCTION__);
            }
        }
    } 

    // 
    // serialize the Message
    //


    asn_r = der_encode(&asn_DEF_BDM_S2C_Message, &m, bionet_accumulate_asn_buffer, buf);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_S2C_Message, &m);
    if (asn_r.encoded == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s(): error with der_encode(): %s", __FUNCTION__, strerror(errno));
        if (buf->buf != NULL) {
            free(buf->buf);
            buf->buf = NULL;
        }
        goto cleanup;
    }

    return 0;


cleanup:
    if (asn_pd) {
	free(asn_pd);
    }
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_S2C_Message, &m);
    return -1;
}

bionet_datapoint_t *bdm_publish_asn_to_datapoint(
        BDMPublishDatapoint_t *asn_pub_dp,
        bionet_resource_t *resource,
        long * ret_seq)
{
    int r;

    bionet_datapoint_t * datapoint = bionet_asn_to_datapoint(&asn_pub_dp->datapoint, resource);
    if(datapoint == NULL) {
        // An error has been logged
        return NULL;
    }

    const char * bdm_id = (const char *)asn_pub_dp->bdmId.buf;

    struct timeval event_timestamp;
    r = bionet_GeneralizedTime_to_timeval(&asn_pub_dp->timestamp, &event_timestamp);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): error setting timestamp: %s", 
                __FUNCTION__, strerror(errno));
        bionet_datapoint_free(datapoint);
        return NULL;
    }

    if(ret_seq) {
        *ret_seq = asn_pub_dp->entrySeq;
    }

    bionet_event_t * event = bionet_event_new(&event_timestamp, bdm_id, BIONET_EVENT_PUBLISHED);
    if(event == NULL) {
        // An error has been logged already
        bionet_datapoint_free(datapoint);
        return NULL;
    }

    bionet_datapoint_add_event(datapoint, event);

    return datapoint;
}

DataManager_t * bionet_bdm_to_asn(bionet_bdm_t * bdm) {
    DataManager_t * asn_bdm = NULL;
    int r;
    int hi;
    GPtrArray * hab_list = NULL;

    asn_bdm = (DataManager_t *)calloc(1, sizeof(DataManager_t));
    if (asn_bdm == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
              "%s(): out of memory!", __FUNCTION__);
        goto cleanup;
    }

    r = OCTET_STRING_fromString(&asn_bdm->id, bionet_bdm_get_id(bdm));
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
              "%s(): error making OCTET_STRING for BDM-ID %s", __FUNCTION__, bionet_bdm_get_id(bdm));
        goto cleanup;
    }
        
    hab_list = bionet_bdm_get_hab_list(bdm);
    for (hi = 0; hi < hab_list->len; hi++) {
        BDM_HardwareAbstractor_t * asn_hab;
        bionet_hab_t * hab = g_ptr_array_index(hab_list, hi);
        if (NULL == hab) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
                  "%s(): Failed to get HAB %d from array of HABs", __FUNCTION__, hi);
            continue;
        }
        
        //add the HAB to the message
        asn_hab = (BDM_HardwareAbstractor_t *)calloc(1, sizeof(BDM_HardwareAbstractor_t));
        if (asn_hab == NULL) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                  "%s(): out of memory!", __FUNCTION__);
            goto cleanup;
        }

        r = bionet_hab_to_bdm_asn(hab, asn_hab);
        if(r != 0) {
            free(asn_hab);
            asn_hab = NULL;
            goto cleanup;
        }

        r = asn_sequence_add(&asn_bdm->hablist.list, asn_hab);
        if (r != 0) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                  "%s(): error adding HAB to Metadata: %s", __FUNCTION__, strerror(errno));
            ASN_STRUCT_FREE(asn_DEF_BDM_HardwareAbstractor, asn_hab);
            goto cleanup;
        }
    } //for (hi = 0; hi < hab_list->len; hi++)

    return asn_bdm;

cleanup:
    if(asn_bdm) {
        ASN_STRUCT_FREE(asn_DEF_DataManager, asn_bdm);
    }
    return NULL;

}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
