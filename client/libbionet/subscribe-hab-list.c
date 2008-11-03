
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <string.h>

#include <glib.h>

#include "bionet.h"
#include "libbionet-internal.h"


int bionet_subscribe_hab_list_by_habtype_habid(const char *hab_type,  const char *hab_id) {
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

