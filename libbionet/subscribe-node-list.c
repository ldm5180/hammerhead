
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <glib.h>

#include "bionet.h"
#include "libbionet-internal.h"


int bionet_subscribe_node_list_by_habtype_habid_nodeid(const char *hab_type,  const char *hab_id, const char *node_id) {
    int r;
    char publisher[BIONET_NAME_COMPONENT_MAX_LEN * 2];
    char topic[BIONET_NAME_COMPONENT_MAX_LEN + 2];  // the +2 is for the leading "N " subscription family specifier
    libbionet_node_subscription_t *new_node_sub;

    new_node_sub = calloc(1, sizeof(libbionet_node_subscription_t));
    if (new_node_sub == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_subscribe_node_list_by_habtype_habid_nodeid(): out of memory");
        goto fail0;
    }

    new_node_sub->hab_type = strdup(hab_type);
    if (new_node_sub->hab_type == NULL)  {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_subscribe_node_list_by_habtype_habid_nodeid(): out of memory");
        goto fail1;
    }

    new_node_sub->hab_id = strdup(hab_id);
    if (new_node_sub->hab_id == NULL)  {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_subscribe_node_list_by_habtype_habid_nodeid(): out of memory");
        goto fail2;
    }

    new_node_sub->node_id = strdup(node_id);
    if (new_node_sub->node_id == NULL)  {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_subscribe_node_list_by_habtype_habid_nodeid(): out of memory");
        goto fail3;
    }

    libbionet_node_subscriptions = g_slist_prepend(libbionet_node_subscriptions, new_node_sub);

    r = snprintf(publisher, sizeof(publisher), "%s.%s", hab_type, hab_id);
    if (r >= sizeof(publisher)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_subscribe_node_list_by_habtype_habid_nodeid(): HAB name '%s.%s' too long", hab_type, hab_id);
        goto fail4;
    }

    r = snprintf(topic, sizeof(topic), "N %s", node_id);
    if (r >= sizeof(topic)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_subscribe_node_list_by_habtype_habid_nodeid(): Node name '%s' too long", node_id);
        goto fail4;
    }

    // send the subscription request to the HAB
    r = cal_client.subscribe(libbionet_cal_handle, publisher, topic);
    if (!r) return -1;

    return 0;

fail4:
    free(new_node_sub->node_id);

fail3:
    free(new_node_sub->hab_id);

fail2:
    free(new_node_sub->hab_type);

fail1:
    free(new_node_sub);

fail0:
    return -1;
}




int bionet_subscribe_node_list_by_name(const char *node_name) {
    char *hab_type;
    char *hab_id;
    char *node_id;
    int r;

    r = bionet_split_node_name(node_name, &hab_type, &hab_id, &node_id);
    if (r != 0) {
        // a helpful error message has already been logged
        return -1;
    }

    return bionet_subscribe_node_list_by_habtype_habid_nodeid(hab_type, hab_id, node_id);
}

