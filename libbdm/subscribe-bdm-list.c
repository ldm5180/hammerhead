
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "libbdm-internal.h"


int bdm_subscribe_bdm_list_by_name(const char *peer_id) {
    libbdm_bdm_subscription_t *new_bdm_sub;

    new_bdm_sub = calloc(1, sizeof(libbdm_bdm_subscription_t));
    if (new_bdm_sub == NULL) {
        goto fail0;
    }

    new_bdm_sub->peer_id = strdup(peer_id);
    if (new_bdm_sub->peer_id == NULL) {
        goto fail1;
    }


    libbdm_bdm_subscriptions = g_slist_prepend(libbdm_bdm_subscriptions, new_bdm_sub);

    // see if this subscriptions reveals any new bdms 
    {
        GHashTableIter iter;
        gpointer key, val;

        g_hash_table_iter_init(&iter, libbdm_all_peers); 
        while(g_hash_table_iter_next(&iter, &key, &val)) {
            libbdm_peer_t * peer = (libbdm_peer_t*)val;

            if (bionet_name_component_matches((const char *)key, peer_id)) {
                if (libbdm_callback_new_bdm != NULL) {
                    bionet_bdm_t * bdm = bdm_cache_lookup_bdm(peer->bdm_id);
                    if(bdm){
                        libbdm_callback_new_bdm(bdm, libbdm_callback_new_bdm_usr_data);
                    }
                }
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
