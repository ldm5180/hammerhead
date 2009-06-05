
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "bionet.h"
#include "libbionet-internal.h"


int bionet_subscribe_stream_by_habtype_habid_nodeid_streamid(const char *hab_type,  const char *hab_id, const char *node_id, const char *stream_id) {
    int r;
    char publisher[BIONET_NAME_COMPONENT_MAX_LEN * 2];
    char topic[(BIONET_NAME_COMPONENT_MAX_LEN * 2) + 2];  // the +2 is for the starting "S " subscription family
    libbionet_datapoint_subscription_t *new_stream_sub;

    new_stream_sub = calloc(1, sizeof(libbionet_datapoint_subscription_t));
    if (new_stream_sub == NULL)
        goto fail0;

    new_stream_sub->hab_type = strdup(hab_type);
    if (new_stream_sub->hab_type == NULL) 
        goto fail1;

    new_stream_sub->hab_id = strdup(hab_id);
    if (new_stream_sub->hab_id == NULL) 
        goto fail2;

    new_stream_sub->node_id = strdup(node_id);
    if (new_stream_sub->node_id == NULL) 
        goto fail3;

    new_stream_sub->resource_id = strdup(stream_id);
    if (new_stream_sub->resource_id == NULL) 
        goto fail4;

    libbionet_stream_subscriptions = g_slist_prepend(libbionet_stream_subscriptions, new_stream_sub);

    r = snprintf(publisher, sizeof(publisher), "%s.%s", hab_type, hab_id);
    if (r >= sizeof(publisher)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_subscribe_stream_by_habtype_habid_nodeid_streamid(): HAB name '%s.%s' too long", hab_type, hab_id);
        return -1;
    }

    r = snprintf(topic, sizeof(topic), "S %s:%s", node_id, stream_id);
    if (r >= sizeof(topic)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_subscribe_stream_by_habtype_habid_nodeid_streamid(): topic 'S %s:%s' too long", node_id, stream_id);
        return -1;
    }

    // send the subscription request to the HAB
    r = cal_client.subscribe(publisher, topic);
    if (!r) return -1;

    return 0;

fail4:
    free(new_stream_sub->node_id);

fail3:
    free(new_stream_sub->hab_id);

fail2:
    free(new_stream_sub->hab_type);

fail1:
    free(new_stream_sub);

fail0:
    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_subscribe_datapoints_by_habtype_habid_nodeid_resourceid(): out of memory");
    return -1;

}




int bionet_subscribe_stream_by_name(const char *stream_name) {
    char *hab_type;
    char *hab_id;
    char *node_id;
    char *stream_id;
    int r;

    r = bionet_split_resource_name(stream_name, &hab_type, &hab_id, &node_id, &stream_id);
    if (r != 0) {
        // a helpful error message has already been logged
        return -1;
    }

    return bionet_subscribe_stream_by_habtype_habid_nodeid_streamid(hab_type, hab_id, node_id, stream_id);
}


