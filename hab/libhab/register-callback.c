
//
// Copyright (C) 2008, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G and NNC06CB40C.
//


#include "hardware-abstractor.h"
#include "libhab-internal.h"


void hab_register_callback_set_resource(void (*cb_set_resource)(const char *node_id, const char *resource_id, const char *value)) {
    libhab_callback_set_resource = cb_set_resource;
}

