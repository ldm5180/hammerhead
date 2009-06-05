
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


int bionet_unsubscribe_stream_by_habtype_habid_nodeid_streamid(const char *hab_type,  const char *hab_id, const char *node_id, const char *stream_id) {
    int r;
    char publisher[BIONET_NAME_COMPONENT_MAX_LEN * 2];
    char topic[(BIONET_NAME_COMPONENT_MAX_LEN * 2) + 2];  // the +2 is for the starting "S " subscription family
    GSList *i = libbionet_stream_subscriptions;

    while (i != NULL) {
        libbionet_datapoint_subscription_t *stream_sub = i->data;

        if (stream_sub == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_unsubscribe_stream...(): NULL stream subscription!");
            i = i->next;
            continue;
        }

        if ((stream_sub->hab_type == NULL) || 
            (stream_sub->hab_id == NULL) ||
            (stream_sub->node_id == NULL) ||
            (stream_sub->resource_id == NULL)) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_unsubscribe_stream...(): NULL stream subscription component!");
            i = i->next;
            continue;
        }

        if (!((strcmp(stream_sub->hab_type, hab_type) == 0) &&
            (strcmp(stream_sub->hab_id, hab_id) == 0) &&
            (strcmp(stream_sub->node_id, node_id) == 0) &&
            (strcmp(stream_sub->resource_id, stream_id) == 0))) {
            i = i->next;
            continue;
        }

        free(stream_sub->hab_type);
        free(stream_sub->hab_id);
        free(stream_sub->node_id);
        free(stream_sub->resource_id);
        free(stream_sub);
        i->data = NULL;
        
        libbionet_stream_subscriptions = g_slist_delete_link(libbionet_stream_subscriptions, i);


        r = snprintf(publisher, sizeof(publisher), "%s.%s", hab_type, hab_id);
        if (r >= sizeof(publisher)) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_unsubscribe_stream_by_habtype_habid_nodeid_streamid(): HAB name '%s.%s' too long", hab_type, hab_id);
            return -1;
        }

        r = snprintf(topic, sizeof(topic), "S %s:%s", node_id, stream_id);
        if (r >= sizeof(topic)) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_unsubscribe_stream_by_habtype_habid_nodeid_streamid(): topic 'S %s:%s' too long", node_id, stream_id);
            return -1;
        }

        // send the subscription request to the HAB
        r = cal_client.unsubscribe(publisher, topic);
        if (!r) return -1;

        return 0;
    }

    errno = ENOENT;
    return -1;
}




int bionet_unsubscribe_stream_by_name(const char *stream_name) {
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

    return bionet_unsubscribe_stream_by_habtype_habid_nodeid_streamid(hab_type, hab_id, node_id, stream_id);
}


