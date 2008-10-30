
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <glib.h>

#include "cal-client.h"

#include "libbionet-internal.h"
#include "bionet.h"




int bionet_read(void) {
    return cal_client.read();
}




#if 0

void bionet_read_from_nag_but_dont_handle_messages(void) {
    if (bionet_connect_to_nag() < 0) {
        return;
    }

    do {
        bionet_message_t *message;
        int r;
        struct timeval timeout;

        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        r = bionet_nxio_read(libbionet_nag_nxio, &timeout, &message);
        if (r < 0) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error reading from NAG: %s", strerror(errno));
            libbionet_kill_nag_connection();
            return;
        }

        if (message == NULL) {
            // nothing from the NAG
            return;
        }

        libbionet_queued_messages_from_nag = g_slist_append(libbionet_queued_messages_from_nag, message);
    } while(1);
}

#endif

