
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>
#include <stdlib.h>


#include "internal.h"
#include "protected.h"

#include <glib.h>

GPtrArray * bionet_bdm_get_hab_list(bionet_bdm_t * bdm) {
    return bdm->hab_list;
}

