
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>

#include <glib.h>

#include "bdm-client.h"
#include "bdm-util.h"
#include "cal-client.h"
#include "libbdm-internal.h"

extern int bdm_fd;

void bdm_disconnect() {
    if (libbdm_cal_fd < 0) return;

    cal_client.shutdown();

    libbdm_cal_fd = -1;
    bdm_fd = -1;

}


