
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "libbdm-internal.h"


int bdm_subscribe_bdm_list_by_name(const char *bdm_name) {
    libbdm_bdm_subscription_t *new_bdm_sub;
    char peer_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char bdm_id[BIONET_NAME_COMPONENT_MAX_LEN];

    if ( bdm_split_bdm_peer_id(bdm_name, peer_id, bdm_id) < 0) {
        // Error already logged
        return -1;
    }

    new_bdm_sub = calloc(1, sizeof(libbdm_bdm_subscription_t));
    if (new_bdm_sub == NULL) {
        goto fail0;
    }

    new_bdm_sub->bdm_id = strdup(bdm_id);
    if (new_bdm_sub->bdm_id == NULL) {
        goto fail1;
    }

    new_bdm_sub->peer_id = strdup(peer_id);
    if (new_bdm_sub->peer_id == NULL) {
        goto fail1;
    }


    libbdm_bdm_subscriptions = g_slist_prepend(libbdm_bdm_subscriptions, new_bdm_sub);

    // see if this subscriptions reveals any new bdms revealed by existing, 
    // more specific subscriptions
    {
        GHashTableIter iter;
        gpointer key, val;

        g_hash_table_iter_init(&iter, libbdm_all_peers); 
        while(g_hash_table_iter_next(&iter, &key, &val)) {
            libbdm_peer_t * peer = (libbdm_peer_t*)val;

            if (bionet_name_component_matches((const char *)key, peer_id)) {
                GSList *b;

                for(b = peer->recording_bdms; b != NULL; b = b->next ) {
                    bionet_bdm_t *bdm = b->data;
                    
                    if (bionet_name_component_matches(bdm->id, bdm_id)) {
                        if (libbdm_callback_new_bdm != NULL) {
                            libbdm_callback_new_bdm(bdm, libbdm_callback_new_bdm_usr_data);
                        }
                    }
                } // for each recording_bdms
                
                // TODO: Send BDM subscription ("B %s", bdm_id) to server.. 
                // Until then, new_bdm callback will be sent when first datapoint arrives
            }
        }
    }

    return 0;

fail1:
    free(new_bdm_sub);

fail0:
    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bdm_subscribe_bdm_list(): out of memory");
    return -1;

}



// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
