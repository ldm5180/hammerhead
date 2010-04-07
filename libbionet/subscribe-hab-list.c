
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "bionet.h"
#include "libbionet-internal.h"


int bionet_subscribe_hab_list_by_habtype_habid(const char *hab_type,  const char *hab_id) {
    libbionet_hab_subscription_t *new_hab_sub;

    new_hab_sub = calloc(1, sizeof(libbionet_hab_subscription_t));
    if (new_hab_sub == NULL) {
        goto fail0;
    }

    new_hab_sub->hab_type = strdup(hab_type);
    if (new_hab_sub->hab_type == NULL) {
        goto fail1;
    }

    new_hab_sub->hab_id = strdup(hab_id);
    if (new_hab_sub->hab_id == NULL) {
        goto fail2;
    }

    libbionet_hab_subscriptions = g_slist_prepend(libbionet_hab_subscriptions, new_hab_sub);

    // see if this subscriptions reveals any new habs
    {
        GSList *i;

        for (i = libbionet_habs; i != NULL; i = i->next) {
            bionet_hab_t *hab = i->data;

            if (bionet_hab_matches_type_and_id(hab, new_hab_sub->hab_type, new_hab_sub->hab_id)) {
                if (bionet_cache_lookup_hab(bionet_hab_get_type(hab), bionet_hab_get_id(hab)) == NULL) {
                    libbionet_cache_add_hab(hab);
                }
                if (libbionet_callback_new_hab != NULL) {
                    libbionet_callback_new_hab(hab);
                }
            }
        }
    }

    return 0;

fail2:
    free(new_hab_sub->hab_type);

fail1:
    free(new_hab_sub);

fail0:
    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_subscribe_hab_list(): out of memory");
    return -1;


#if 0
    int r;

    bionet_message_t m;


    if (hab_type == NULL) hab_type = "*";
    if (hab_id   == NULL) hab_id   = "*";

    // FIXME: here check the strings for validity ([-a-zA-Z0-9])


    if (bionet_connect_to_nag() < 0) {
        return -1;
    }


    m.type = Bionet_Message_C2N_Subscribe_HAB_List;
    m.body.c2n_subscribe_hab_list.hab_type = (char *)hab_type;
    m.body.c2n_subscribe_hab_list.hab_id = (char *)hab_id;

    r = bionet_nxio_send_message(libbionet_nag_nxio, &m);
    if (r < 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_subscribe_hab_list(): error sending request to NAG");
        return -1;
    }

    r = libbionet_read_ok_from_nag();
    if (r != 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_subscribe_hab_list(): error reading reply from NAG");
        libbionet_kill_nag_connection();
        return -1;
    }

    return 0;
#endif
}




int bionet_subscribe_hab_list_by_name(const char *hab_name) {
    char *hab_type;
    char *hab_id;
    int r;

    r = bionet_split_hab_name(hab_name, &hab_type, &hab_id);
    if (r != 0) {
        // a helpful log message has already been logged
        return -1;
    }

    return bionet_subscribe_hab_list_by_habtype_habid(hab_type, hab_id);
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
