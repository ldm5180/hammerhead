
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




int bionet_read_with_timeout(struct timeval *timeout) {
    int r;
    fd_set readers;

    if (!bionet_is_connected()) return -1;

    FD_ZERO(&readers);
    FD_SET(libbionet_cal_fd, &readers);

    r = select(libbionet_cal_fd + 1, &readers, NULL, NULL, timeout);
    if (r < 0) return r;

    // FIXME: CAL return TRUE on success, FALSE on failure.
    r = cal_client.read();
    if (r) return 0;
    return -1;
}


int bionet_read(void) {
    return cal_client.read();
}

