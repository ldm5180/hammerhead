
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#define _SVID_SOURCE //needed for strdup
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <errno.h>

#include "bionet-data-manager.h"
#include "bionet-util.h"

#include "BDMResourceMetadata.h"
#include "BDMResourceDatapoints.h"
#include "BDM-S2C-Message.h"


int bdm_new_hab_to_asnbuf(
        bionet_hab_t *hab,
        long entry_seq,
        bionet_asn_buffer_t *buf) 
{
    BDM_S2C_Message_t m;
    BDMNewHab_t *nh;
    asn_enc_rval_t asn_r;

    int r;
    buf->buf = NULL;
    buf->size = 0;

    memset(&m, 0x00, sizeof(m));
    m.present = BDM_S2C_Message_PR_newHab;
    nh = &m.choice.newHab;

    r = OCTET_STRING_fromBuf(&nh->bdmId, bionet_hab_get_recording_bdm(hab), -1);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set bdm id", __FUNCTION__);
        goto cleanup;
    }

    nh->entrySeq = entry_seq;

    r = bionet_hab_to_asn(hab, &nh->hab);
    if (r != 0) {
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

int bdm_new_node_to_asnbuf(
        bionet_node_t *node,
        long entry_seq,
        bionet_asn_buffer_t *buf) 
{
    BDM_S2C_Message_t m;
    BDMNewNode_t *nn;
    asn_enc_rval_t asn_r;
    bionet_hab_t * hab;

    int r;
    buf->buf = NULL;
    buf->size = 0;

    hab = bionet_node_get_hab(node);

    memset(&m, 0x00, sizeof(m));
    m.present = BDM_S2C_Message_PR_newNode;
    nn = &m.choice.newNode;

    r = OCTET_STRING_fromBuf(&nn->bdmId, bionet_hab_get_recording_bdm(hab), -1);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set bdm id", __FUNCTION__);
        goto cleanup;
    }


    r = OCTET_STRING_fromBuf(&nn->habType, bionet_hab_get_type(hab), -1);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set hab type", __FUNCTION__);
        goto cleanup;
    }

    r = OCTET_STRING_fromBuf(&nn->habId, bionet_hab_get_id(hab), -1);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set hab id", __FUNCTION__);
        goto cleanup;
    }

    nn->entrySeq = entry_seq;

    bionet_node_to_asn(node, &nn->node);


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

    r = OCTET_STRING_fromBuf(&rm->bdmId, bionet_hab_get_recording_bdm(hab), -1);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set bdm id", __FUNCTION__);
        goto cleanup;
    }


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

    rm->entrySeq = entry_seq;

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


    buf->buf = NULL;
    buf->size = 0;

    node = bionet_resource_get_node(resource);
    hab = bionet_node_get_hab(node);

    memset(&m, 0x00, sizeof(m));
    m.present = BDM_S2C_Message_PR_datapointsUpdate;
    rd = &m.choice.datapointsUpdate;

    r = OCTET_STRING_fromBuf(&rd->bdmId, bionet_hab_get_recording_bdm(hab), -1);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set bdm id", __FUNCTION__);
        goto cleanup;
    }

    r = OCTET_STRING_fromBuf(&rd->habType, bionet_hab_get_type(hab), -1);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set hab type", __FUNCTION__);
        goto cleanup;
    }

    r = OCTET_STRING_fromBuf(&rd->habId, bionet_hab_get_id(hab), -1);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set hab id", __FUNCTION__);
        goto cleanup;
    }

    r = OCTET_STRING_fromBuf(&rd->nodeId, bionet_node_get_id(node), -1);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): Failed to set node id", __FUNCTION__);
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
        bionet_datapoint_t * d = bionet_resource_get_datapoint_by_index(resource, di);
        Datapoint_t *asn_d;
        if (NULL == d) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
                  "%s(): Failed to get datapoint %d from Resource %s", __FUNCTION__,
                  di, bionet_resource_get_name(resource));
        }
        
        asn_d = bionet_datapoint_to_asn(d);
        if (asn_d == NULL) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                  "%s(): out of memory!", __FUNCTION__);
        }
        
        r = asn_sequence_add(&rd->newDatapointsBDM.list, asn_d);
        if (r != 0) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                  "%s(): error adding Datapoint to Resource: %m", __FUNCTION__);
        }
    } 

    rd->entrySeq = entry_seq;

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
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_BDM_S2C_Message, &m);
    return -1;
}



// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
