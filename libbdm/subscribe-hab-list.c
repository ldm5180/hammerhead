
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "libbdm-internal.h"


int bdm_subscribe_hab_list_by_name(const char *hab_name) {
    libbdm_hab_subscription_t *new_hab_sub;
    char peer_id[BIONET_NAME_COMPONENT_MAX_LEN+1];
    char bdm_id[BIONET_NAME_COMPONENT_MAX_LEN+1];
    char hab_type[BIONET_NAME_COMPONENT_MAX_LEN+1];
    char hab_id[BIONET_NAME_COMPONENT_MAX_LEN+1];
    char topic[BDM_TOPIC_MAX_LEN+1];

    if ( 0 != bdm_split_hab_name_r(hab_name, peer_id, bdm_id, hab_type, hab_id) ) {
        return -1;
    }

    new_hab_sub = calloc(1, sizeof(libbdm_hab_subscription_t));
    if (new_hab_sub == NULL) {
        goto fail0;
    }

    new_hab_sub->peer_id = strdup(peer_id);
    if (new_hab_sub->peer_id == NULL) {
        goto fail1;
    }

    new_hab_sub->bdm_id = strdup(bdm_id);
    if (new_hab_sub->bdm_id == NULL) {
        goto fail1;
    }

    new_hab_sub->hab_type = strdup(hab_type);
    if (new_hab_sub->hab_type == NULL) {
        goto fail1;
    }

    new_hab_sub->hab_id = strdup(hab_id);
    if (new_hab_sub->hab_id == NULL) {
        goto fail1;
    }

    libbdm_hab_subscriptions = g_slist_prepend(libbdm_hab_subscriptions, new_hab_sub);

    // Send this subscription to CAL
    if(snprintf(topic, sizeof(topic), "H %s", hab_name) < 0 ) {
        goto fail1;
    }

    if ( !cal_client.subscribe(libbdm_cal_handle, peer_id, topic) ) return -1;

    return 0;

fail1:
    free(new_hab_sub->peer_id);
    free(new_hab_sub->bdm_id);
    free(new_hab_sub->hab_type);
    free(new_hab_sub->hab_id);
    free(new_hab_sub);

fail0:
    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): out of memory", __FUNCTION__);
    return -1;

}



// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
