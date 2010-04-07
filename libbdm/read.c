
// Copyright (c) 2008-2010, Regents of the University of Colorado.
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

#include "libbdm-internal.h"

int bdm_read_with_timeout(struct timeval *timeout) {
    int r;

    if (libbdm_cal_handle == NULL) return -1;

    r = cal_client.read(libbdm_cal_handle, timeout);
    if (r) return 0;
    return -1;
}


int bdm_read(void) {
    struct timeval timeout;

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    return cal_client.read(libbdm_cal_handle, &timeout);
}

