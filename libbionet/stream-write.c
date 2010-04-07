
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "cal-client.h"

#include "libbionet-internal.h"
#include "bionet.h"
#include "bionet-asn.h"


void bionet_stream_write(bionet_stream_t *stream, const void *buf, unsigned int size) {
    bionet_hab_t *hab;
    bionet_node_t *node;
    int r;

    C2H_Message_t m;
    StreamData_t *sd;
    bionet_asn_buffer_t asn_buf;
    asn_enc_rval_t asn_r;

    // sanity checks
    if (stream == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_write: NULL Stream passed in");
	return;
    }


    node = bionet_stream_get_node(stream);

    memset(&m, 0, sizeof(C2H_Message_t));
    memset(&asn_buf, 0, sizeof(bionet_asn_buffer_t));

    m.present = C2H_Message_PR_streamData;
    sd = &m.choice.streamData;

    r = OCTET_STRING_fromString(&sd->nodeId, bionet_node_get_id(node));
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_write(): error making OCTET_STRING for Node-ID %s", bionet_node_get_id(node));
        goto cleanup;;
    }

    r = OCTET_STRING_fromString(&sd->streamId, bionet_stream_get_id(stream));
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_write(): error making OCTET_STRING for Stream-ID %s", bionet_stream_get_id(stream));
        goto cleanup;
    }

    r = OCTET_STRING_fromBuf(&sd->data, buf, size);
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_write(): error making OCTET_STRING for %d bytes of Stream Data", size);
        goto cleanup;
    }

    hab = bionet_stream_get_hab(stream);


    //
    // serialize the ASN.1 message
    //

    asn_r = der_encode(&asn_DEF_C2H_Message, &m, bionet_accumulate_asn_buffer, &asn_buf);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_C2H_Message, &m);
    if (asn_r.encoded == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_stream_write(): error with der_encode(): %s", strerror(errno));
        if (asn_buf.buf != NULL) free(asn_buf.buf);
        return;
    }


    // send the command to the HAB
    // Note: cal_client.sendto steals the dynamically-allocated buffer
    cal_client.sendto(libbionet_cal_handle, bionet_hab_get_name(hab), asn_buf.buf, asn_buf.size);

    return;


cleanup:
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_C2H_Message, &m);
    if (asn_buf.buf != NULL) free(asn_buf.buf);
}

