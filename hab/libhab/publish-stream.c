
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>

#include <glib.h>

#include "hardware-abstractor.h"

#include "libhab-internal.h"
#include "bionet-asn.h"


int hab_publish_stream(const bionet_stream_t *stream, const void *buf, size_t count) {
    char topic[BIONET_NAME_COMPONENT_MAX_LEN + 2];
    bionet_node_t *node;
    int r;

    asn_enc_rval_t asn_r;
    H2C_Message_t m;
    StreamData_t *sd;
    bionet_asn_buffer_t asnbuf;


    node = bionet_stream_get_node(stream);
    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_publish_stream: passed-in Stream has NULL Node");
        goto fail0;
    }


    // 
    // make the ASN.1 message containing the stream data
    //

    memset(&asnbuf, 0x00, sizeof(bionet_asn_buffer_t));

    memset(&m, 0x00, sizeof(H2C_Message_t));
    m.present = H2C_Message_PR_streamData;
    sd = &m.choice.streamData;

    r = OCTET_STRING_fromString(&sd->nodeId, bionet_node_get_id(node));
    if (r != 0){
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_publish_stream: error making ASN.1 message from passed-in stream data");
        goto fail0;
    }

    r = OCTET_STRING_fromString(&sd->streamId, bionet_stream_get_id(stream));
    if (r != 0){
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_publish_stream: error making ASN.1 message from passed-in stream data");
        goto fail0;
    }

    r = OCTET_STRING_fromBuf(&sd->data, buf, count);
    if (r != 0){
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_publish_stream: error making ASN.1 message from passed-in stream data");
        goto fail0;
    }

    asn_r = der_encode(&asn_DEF_H2C_Message, &m, bionet_accumulate_asn_buffer, &asnbuf);
    if (asn_r.encoded == -1) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_lost_node(): error with der_encode(): %s", strerror(errno));
        goto fail0;
    }


    // 
    // make the publish topic
    //

    node = bionet_stream_get_node(stream);
    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_publish_stream: passed-in Stream has NULL Node!");
        goto fail1;
    }
    snprintf(topic, BIONET_NAME_COMPONENT_MAX_LEN * 2 + 1,
	     "S %s", bionet_stream_get_local_name(stream));


    // publish
    cal_server.publish(topic, asnbuf.buf, asnbuf.size);

    // FIXME: cal should take buf
    free(asnbuf.buf);

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_H2C_Message, &m);
    return 0;

fail1:
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_H2C_Message, &m);

fail0:
    return -1;
}


