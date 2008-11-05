
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include "bionet-util.h"


int bionet_datapoint_is_dirty(const bionet_datapoint_t *datapoint) {
    return datapoint->dirty;
}

