
//
// Copyright (C) 2008-2009, Gary Grobe and the Regents of the University of Colorado.
//


#include <hardware-abstractor.h>

void cb_set_resource(bionet_resource_t *resource, bionet_value_t *value) {
    g_message(
        "should set %s to %s",
        bionet_resource_get_name(resource),
        bionet_value_to_str(value)
    );
}

