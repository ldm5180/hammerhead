

//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include "bionet-util.h"


void bionet_datapoint_make_clean(bionet_datapoint_t *datapoint) {
    datapoint->dirty = 0;
}

