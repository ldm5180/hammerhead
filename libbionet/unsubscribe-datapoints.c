
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "bionet.h"
#include "libbionet-internal.h"


int bionet_unsubscribe_datapoints_by_habtype_habid_nodeid_resourceid(const char *hab_type,  const char *hab_id, const char *node_id, const char *resource_id) {
    int r;
    char publisher[BIONET_NAME_COMPONENT_MAX_LEN * 2];
    char topic[(BIONET_NAME_COMPONENT_MAX_LEN * 2) + 2];  // the +2 is one for the leading "D " to specify the subscription family
    GSList *i = libbionet_datapoint_subscriptions;

    while (i != NULL) {
        libbionet_datapoint_subscription_t *dp_sub = i->data;

        if (dp_sub == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_unsubscribe_datapoints: encountered a NULL subscription");
            i = i->next;
            continue;
        }

        if ((dp_sub->hab_type == NULL) || (dp_sub->hab_id == NULL) || 
            (dp_sub->node_id == NULL) || (dp_sub->node_id == NULL)) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_unsubscribe_datapoints: encountered a NULL subscription component");
            i = i->next;
            continue;
        }

        if (!((strcmp(dp_sub->hab_type, hab_type) == 0) &&
            (strcmp(dp_sub->hab_id, hab_id) == 0) &&
            (strcmp(dp_sub->node_id, node_id) == 0) &&
            (strcmp(dp_sub->resource_id, resource_id) == 0))) {
            i = i->next;
            continue;
        }

        free(dp_sub->hab_type);
        free(dp_sub->hab_id);
        free(dp_sub->node_id);
        free(dp_sub->resource_id);
        free(dp_sub);
        i->data = NULL;
        
        libbionet_datapoint_subscriptions = g_slist_delete_link(libbionet_datapoint_subscriptions, i);

        libbionet_cache_cleanup_nodes();

        r = snprintf(publisher, sizeof(publisher), "%s.%s", hab_type, hab_id);
        if (r >= sizeof(publisher)) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_unsubscribe_datapoints_by_habtype_habid_nodeid_resourceid(): HAB name '%s.%s' too long", hab_type, hab_id);
            return -1;
        }

        r = snprintf(topic, sizeof(topic), "D %s:%s", node_id, resource_id);
        if (r >= sizeof(topic)) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_unsubscribe_datapoints_by_habtype_habid_nodeid_resourceid(): topic '%s:%s' too long", node_id, resource_id);
            return -1;
        }

        // send the subscription request to the HAB
        r = cal_client.unsubscribe(libbionet_cal_handle, publisher, topic);
        if (!r) return -1;

        return 0;
    }

    errno = ENOENT;
    return -1;
}




int bionet_unsubscribe_datapoints_by_name(const char *resource_name) {
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

    return bionet_unsubscribe_datapoints_by_habtype_habid_nodeid_resourceid(hab_type, hab_id, node_id, resource_id);
}

