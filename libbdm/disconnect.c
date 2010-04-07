
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>

#include <glib.h>

#include "bdm-client.h"
#include "cal-client.h"
#include "libbdm-internal.h"

void bdm_disconnect() {
    if (libbdm_cal_handle == NULL) return;

    cal_client.shutdown(libbdm_cal_handle);

    libbdm_cal_handle = NULL;
    bdm_fd = -1;

}


