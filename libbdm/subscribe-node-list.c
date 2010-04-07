
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "libbdm-internal.h"


int bdm_subscribe_node_list_by_name(const char *node_name) {
    libbdm_node_subscription_t *new_node_sub;

    char peer_id[BIONET_NAME_COMPONENT_MAX_LEN+1];
    char bdm_id[BIONET_NAME_COMPONENT_MAX_LEN+1];
    char hab_type[BIONET_NAME_COMPONENT_MAX_LEN+1];
    char hab_id[BIONET_NAME_COMPONENT_MAX_LEN+1];
    char node_id[BIONET_NAME_COMPONENT_MAX_LEN+1];
    char topic[BDM_TOPIC_MAX_LEN+1];

    if ( 0 != bdm_split_node_name_r(node_name, peer_id, bdm_id, hab_type, hab_id, node_id) ) {
        return -1;
    }

    new_node_sub = calloc(1, sizeof(libbdm_node_subscription_t));
    if (new_node_sub == NULL) {
        goto fail0;
    }

    new_node_sub->peer_id = strdup(peer_id);
    if (new_node_sub->peer_id == NULL) {
        goto fail1;
    }
    new_node_sub->bdm_id = strdup(bdm_id);
    if (new_node_sub->bdm_id == NULL) {
        goto fail1;
    }
    new_node_sub->hab_type = strdup(hab_type);
    if (new_node_sub->hab_type == NULL) {
        goto fail1;
    }
    new_node_sub->hab_id = strdup(hab_id);
    if (new_node_sub->hab_id == NULL) {
        goto fail1;
    }
    new_node_sub->node_id = strdup(node_id);
    if (new_node_sub->node_id == NULL) {
        goto fail1;
    }

    libbdm_node_subscriptions = g_slist_prepend(libbdm_node_subscriptions, new_node_sub);

    // Send this subscription to CAL
    if(snprintf(topic, sizeof(topic), "N %s", node_name) < 0 ) {
        goto fail1;
    }

    if ( !cal_client.subscribe(libbdm_cal_handle, peer_id, topic) ) return -1;

    return 0;

fail1:

    free(new_node_sub->peer_id);
    free(new_node_sub->bdm_id);
    free(new_node_sub->hab_type);
    free(new_node_sub->hab_id);
    free(new_node_sub->node_id);
    free(new_node_sub);

fail0:
    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): out of memory", __FUNCTION__);
    return -1;

}



// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
