
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <string.h>

#include <glib.h>

#include "hardware-abstractor.h"

#include "libhab-internal.h"
#include "bionet-asn.h"


int hab_report_new_node(const bionet_node_t *node) {
    H2C_Message_t m;
    Node_t *newnode;
    asn_enc_rval_t asn_r;
    int r;

    bionet_asn_buffer_t buf;


    //
    // sanity checks
    //

    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_new_node(): NULL Node passed in");
        goto fail0;
    }

    if (node->hab != libhab_this) {
        g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "hab_report_new_node(): passed-in Node does not belong to this HAB (it belongs to %s.%s, this hab is %s.%s)",
            node->hab->type,
            node->hab->id,
            libhab_this->type,
            libhab_this->id
        );
        goto fail0;
    }

    if (bionet_hab_get_node_by_id(libhab_this, node->id) != node) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_new_node(): passed-in Node not found in this HAB");
        goto fail0;
    }


    memset(&buf, 0x00, sizeof(bionet_asn_buffer_t));

    memset(&m, 0x00, sizeof(H2C_Message_t));
    m.present = H2C_Message_PR_newNode;
    newnode = &m.choice.newNode;


    r = OCTET_STRING_fromString(&newnode->id, node->id);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_new_node(): error making OCTET_STRING for Node-ID %s", node->id);
        goto fail1;
    }

    // publish the message to any connected subscribers
    asn_r = der_encode(&asn_DEF_H2C_Message, &m, bionet_accumulate_asn_buffer, &buf);
    if (asn_r.encoded == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_new_node(): error with der_encode(): %s", strerror(errno));
        goto fail1;
    }

    cal_server.publish(node->id, buf.buf, buf.size);

    // FIXME: cal_server.publish should take the buf
    free(buf.buf);

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_H2C_Message, &m);
    return 0;

fail1:
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_H2C_Message, &m);

fail0:
    return -1;


#if 0
    int r;
    GSList *i;

    bionet_message_t m;


    if (hab_connect_to_nag() < 0) return -1;

    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_new_node(): tried to report NULL node to nag");
        return -1;
    }
    
    if ( bionet_is_valid_name_component(node->id) != 1 ) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_new_node(): tried to report node with invalid id '%s'", node->id);
        return -1;
    }

    //
    // build the message and send it
    //

    m.type = Bionet_Message_H2N_Node;
    m.body.h2n_node.node = (bionet_node_t *)node;

    r = bionet_nxio_send_message(libhab_nag_nxio, &m);
    if (r < 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_new_node(): error sending new node report to NAG");
        return -1;
    }


    //
    // un-dirty all the resources
    //

    for (i = node->resources; i != NULL; i = i->next) {
        bionet_resource_t *resource = i->data;
        resource->dirty = 0;
    }


    // 
    // read the response from the NAG
    //

    r = libhab_read_ok_from_nag();
    if (r == 0) {
        return 0;
    }

    {
        const char *nag_error;

        nag_error = hab_get_nag_error();
        if (nag_error == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_new_node(): error reading response from NAG");
        } else {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_new_node(): error from NAG: %s", nag_error);
        }
    }

    return -1;
#endif
}

