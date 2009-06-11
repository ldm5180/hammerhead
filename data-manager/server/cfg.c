
/*
 * Copyright (c) 2008-2009, Regents of the University of Colorado.
 * This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
 * NNC07CB47C.
 */

#include "bionet-data-manager.h"

sync_sender_config_t * read_config_file(const char * fname) {

    sync_sender_config_t * cfg = calloc(1, sizeof(sync_sender_config_t));
    if(cfg){
        cfg->sync_recipient = strdup("localhost");
    }

    return cfg;
    
}
