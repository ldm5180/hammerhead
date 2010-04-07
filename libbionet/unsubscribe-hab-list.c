
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <glib.h>

#include "bionet.h"
#include "libbionet-internal.h"

extern GSList *bionet_habs;

int bionet_unsubscribe_hab_list_by_habtype_habid(const char *hab_type,  const char *hab_id) {
    GSList *cursor = libbionet_hab_subscriptions;

    // 
    // if the hab type/id are invalid return an error
    //
    
    if ((bionet_is_valid_name_component_or_wildcard(hab_type) != 1) || 
        (bionet_is_valid_name_component_or_wildcard(hab_id) != 1)) {
        errno = EINVAL;
        return -1;
    }

    while (cursor != NULL) {
        GSList *link = cursor;
        cursor = cursor->next;

        libbionet_hab_subscription_t *hab_sub = (libbionet_hab_subscription_t*)link->data;

        if (hab_sub == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_unsubscribe_hablist...(): NULL hab subscription!");
            continue;
        }

        if ((hab_sub->hab_type == NULL) || (hab_sub->hab_id == NULL)) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_unsubscribe_hablist...(): NULL hab subscription component!");
            continue;
        }

        if ((strcmp(hab_sub->hab_type, hab_type) == 0) && (strcmp(hab_sub->hab_id, hab_id) == 0)) {
            libbionet_hab_subscriptions = g_slist_remove_link(libbionet_hab_subscriptions, link);

            //
            // remove the matching subscription
            //

            free(hab_sub->hab_type);
            free(hab_sub->hab_id);
            free(hab_sub);
            link->data = NULL;
            g_slist_free(link);

            //
            // send lost_hab callback for all habs that only match this subscription
            //

            if (libbionet_callback_lost_hab != NULL) {
                GSList *i;
                for (i = bionet_habs; i != NULL; i = i->next) {
                    GSList *j;
                    bionet_hab_t *hab = i->data;
                    int found_matching_sub = 0;

                    if (hab == NULL) {
                        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_unsubscribe_hablist...(): NULL hab exists in cache");
                        continue;
                    }

                    // make sure there are no other subscriptions to this hab
                    for (j = libbionet_hab_subscriptions; j != NULL; j = j->next) {
                        libbionet_hab_subscription_t *sub = j->data;

                        if ((sub == NULL) || (sub->hab_type == NULL) || (sub->hab_id == NULL))
                            continue;

                        if (bionet_hab_matches_type_and_id(hab, sub->hab_type, sub->hab_id)) {
                            found_matching_sub = 1;
                            break;
                        }
                    }
                    
                    if ( !found_matching_sub ) 
                        libbionet_callback_lost_hab(hab);
                }
            }

            //
            // cleanup the libbionet cache
            //

            libbionet_cache_cleanup_habs();

            return 0;
        }
    }

    errno = ENOENT;
    return -1;
}




int bionet_unsubscribe_hab_list_by_name(const char *hab_name) {
    char *hab_type;
    char *hab_id;
    int r;

    r = bionet_split_hab_name(hab_name, &hab_type, &hab_id);
    if (r != 0) {
        // a helpful log message has already been logged
        errno = EINVAL;
        return -1;
    }

    return bionet_unsubscribe_hab_list_by_habtype_habid(hab_type, hab_id);
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
