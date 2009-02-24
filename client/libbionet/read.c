
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


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




int bionet_read_with_timeout(struct timeval *timeout) {
    int r;

    if (!bionet_is_connected()) return -1;

    r = cal_client.read(timeout);
    if (r) return 0;
    return -1;
}


int bionet_read(void) {
    struct timeval timeout;

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    return cal_client.read(&timeout);
}

