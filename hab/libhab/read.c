
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include "cal-server.h"
#include "libhab-internal.h"
#include "hardware-abstractor.h"


void hab_read(void) {
    struct timeval timeout;

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    cal_server.read(&timeout);
} /* hab_read() */


int hab_read_with_timeout(struct timeval *timeout) {
    int r;

    r = cal_server.read(timeout);
    if (r) return 0;
    return -1;
} /* hab_read_with_timeout() */
