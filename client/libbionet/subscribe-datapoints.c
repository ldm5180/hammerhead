
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


int bionet_subscribe_datapoints_by_habtype_habid_nodeid_resourceid(const char *hab_type,  const char *hab_id, const char *node_id, const char *resource_id) {
    int r;
    char publisher[BIONET_NAME_COMPONENT_MAX_LEN * 2];
    char topic[(BIONET_NAME_COMPONENT_MAX_LEN * 2) + 2];  // the +2 is one for the leading "D " to specify the subscription family
    libbionet_datapoint_subscription_t *new_datapoint_sub;

    new_datapoint_sub = calloc(1, sizeof(libbionet_datapoint_subscription_t));
    if (new_datapoint_sub == NULL)
        goto fail0;

    new_datapoint_sub->hab_type = strdup(hab_type);
    if (new_datapoint_sub->hab_type == NULL) 
        goto fail1;

    new_datapoint_sub->hab_id = strdup(hab_id);
    if (new_datapoint_sub->hab_id == NULL) 
        goto fail2;

    new_datapoint_sub->node_id = strdup(node_id);
    if (new_datapoint_sub->node_id == NULL) 
        goto fail3;

    new_datapoint_sub->resource_id = strdup(resource_id);
    if (new_datapoint_sub->resource_id == NULL) 
        goto fail4;

    libbionet_datapoint_subscriptions = g_slist_prepend(libbionet_datapoint_subscriptions, new_datapoint_sub);

    r = snprintf(publisher, sizeof(publisher), "%s.%s", hab_type, hab_id);
    if (r >= sizeof(publisher)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_subscribe_datapoints_by_habtype_habid_nodeid_resourceid(): HAB name '%s.%s' too long", hab_type, hab_id);
        return -1;
    }

    r = snprintf(topic, sizeof(topic), "D %s:%s", node_id, resource_id);
    if (r >= sizeof(topic)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_subscribe_datapoints_by_habtype_habid_nodeid_resourceid(): topic '%s:%s' too long", node_id, resource_id);
        return -1;
    }

    // send the subscription request to the HAB
    r = cal_client.subscribe(publisher, topic);
    if (!r) return -1;

    return 0;

fail4:
    free(new_datapoint_sub->node_id);

fail3:
    free(new_datapoint_sub->hab_id);

fail2:
    free(new_datapoint_sub->hab_type);

fail1:
    free(new_datapoint_sub);

fail0:
    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_subscribe_datapoints_by_habtype_habid_nodeid_resourceid(): out of memory");
    return -1;
}




int bionet_subscribe_datapoints_by_name(const char *resource_name) {
    char *hab_type;
    char *hab_id;
    char *node_id;
    char *resource_id;
    int r;

    r = bionet_split_resource_name(resource_name, &hab_type, &hab_id, &node_id, &resource_id);
    if (r != 0) {
        // a helpful error message has already been logged
        return -1;
    }

    return bionet_subscribe_datapoints_by_habtype_habid_nodeid_resourceid(hab_type, hab_id, node_id, resource_id);
}

