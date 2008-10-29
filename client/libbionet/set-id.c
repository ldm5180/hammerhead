
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>


#include "libbionet-internal.h"

#include "bionet.h"




int bionet_set_id(const char *new_id) {
    if (libbionet_client_id != NULL) free(libbionet_client_id);

    if (new_id == NULL) {
        libbionet_client_id = NULL;
        return 0;
    }

    libbionet_client_id = strdup(new_id);
    if (libbionet_client_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bionet_set_id(): out of memory while setting Id!");
        return -1;
    }

    return 0;
}


