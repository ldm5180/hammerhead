
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "cal-client.h"

#include "libbionet-internal.h"
#include "bionet.h"



void bionet_disconnect() {
    if (libbionet_cal_handle == NULL) return;

    cal_client.shutdown(libbionet_cal_handle);

    libbionet_cal_handle = NULL;
}

