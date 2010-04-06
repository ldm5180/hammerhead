
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "cal-mdnssd-bip.h"

#include <stdlib.h>
#include <glib.h>
#include <pthread.h>

bip_shared_config_t bip_shared_cfg;
pthread_mutex_t avahi_mutex = PTHREAD_MUTEX_INITIALIZER;

void bip_shared_config_init(void) {

    bip_shared_cfg.max_write_buf_size = BIP_MAX_WRITE_BUF_SIZE;

    char * str = getenv("BIP_WRITE_BUFSIZE");
    if(str) {
        char * endptr;
        long x = strtol(str, &endptr, 10);

        if(x > 0 && endptr > str && *endptr == '\0') {
            bip_shared_cfg.max_write_buf_size = x;
            //g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_INFO, 
            //        "Setting BIP_WRITE_BUFSIZE to %ld", x);
        }
    }
}


