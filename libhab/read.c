
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "cal-server.h"
#include "libhab-internal.h"
#include "hardware-abstractor.h"


void hab_read(void) {
    struct timeval timeout;

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    cal_server.read(libhab_cal_handle, &timeout);
} /* hab_read() */


int hab_read_with_timeout(struct timeval *timeout) {
    int r;

    r = cal_server.read(libhab_cal_handle, timeout);
    if (r) return 0;
    return -1;
} /* hab_read_with_timeout() */
