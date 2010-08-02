
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>

#include <glib.h>

#include "hardware-abstractor.h"

#include "libhab-internal.h"
#include "bionet-asn.h"


int hab_report_lost_node(const bionet_node_t * node) {
    H2C_Message_t m;
    PrintableString_t *lostnode;
    asn_enc_rval_t asn_r;
    int r;
    char topic[BIONET_NAME_COMPONENT_MAX_LEN + 2];
    int i;
    bionet_asn_buffer_t buf;
    const char * node_id;

    //
    // sanity checks
    //

    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_lost_node(): NULL Node passed in");
        goto fail0;
    }

    node_id = bionet_node_get_id(node);

    if (bionet_hab_get_node_by_id(libhab_this, node_id) != NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_lost_node(): passed-in Node still exists in this HAB");
        goto fail0;
    }

    for (i = 0; i < bionet_node_get_num_resources(node); i++) {
	bionet_resource_t * resource = bionet_node_get_resource_by_index(node, i);
	if (resource) {
	    bionet_pthread_mutex_lock(&published_hash_mutex);
	    g_hash_table_remove(libhab_most_recently_published, resource);
	    bionet_pthread_mutex_unlock(&published_hash_mutex);
	}
    }

    memset(&buf, 0x00, sizeof(bionet_asn_buffer_t));

    memset(&m, 0x00, sizeof(H2C_Message_t));
    m.present = H2C_Message_PR_lostNode;
    lostnode = &m.choice.lostNode;

    r = OCTET_STRING_fromString(lostnode, node_id);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_lost_node(): error making OCTET_STRING for Node-ID %s", node_id);
        goto fail1;
    }

    // publish the message to any connected subscribers
    asn_r = der_encode(&asn_DEF_H2C_Message, &m, bionet_accumulate_asn_buffer, &buf);
    if (asn_r.encoded == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_lost_node(): error with der_encode(): %s", strerror(errno));
        goto fail1;
    }

    snprintf(topic, sizeof(topic), "N %s", node_id);

    cal_server.publish(libhab_cal_handle, topic, buf.buf, buf.size);

    // FIXME: cal_server.publish should take the buf
    free(buf.buf);

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_H2C_Message, &m);
    return 0;


fail1:
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_H2C_Message, &m);

fail0:
    return -1;
}


