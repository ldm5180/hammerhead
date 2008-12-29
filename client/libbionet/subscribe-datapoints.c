
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>

#include "bionet.h"
#include "libbionet-internal.h"


int bionet_subscribe_datapoints_by_habtype_habid_nodeid_resourceid(const char *hab_type,  const char *hab_id, const char *node_id, const char *resource_id) {
    int r;
    char publisher[(BIONET_NAME_COMPONENT_MAX_LEN * 2) + 2];  // the +2 is one for the '.' and one for the '\0'
    char topic[(BIONET_NAME_COMPONENT_MAX_LEN * 2) + 2];  // the +2 is one for the ':' and one for the '\0'

    r = snprintf(publisher, sizeof(publisher), "%s.%s", hab_type, hab_id);
    if (r >= sizeof(publisher)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_subscribe_datapoints_by_habtype_habid_nodeid_resourceid(): HAB name '%s.%s' too long", hab_type, hab_id);
        return -1;
    }

    r = snprintf(topic, sizeof(topic), "%s:%s", node_id, resource_id);
    if (r >= sizeof(topic)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_subscribe_datapoints_by_habtype_habid_nodeid_resourceid(): topic '%s:%s' too long", node_id, resource_id);
        return -1;
    }

    // send the subscription request to the HAB
    r = cal_client.subscribe(publisher, topic);
    if (!r) return -1;

    return 0;
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

