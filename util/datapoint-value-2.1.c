
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#define _GNU_SOURCE  // for strtof(3)

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "bionet-util.h"




void bionet_datapoint_set_value(bionet_datapoint_t *d, const bionet_datapoint_value_t *value) {
    if (d == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_set_value(): NULL datapoint passed in");
        return;
    }
    if (value == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_set_value(): NULL value passed in");
        return;
    }

    if (d->resource->data_type == BIONET_RESOURCE_DATA_TYPE_STRING) {
        free(d->value.string_v);
        d->value.string_v = strdup(value->string_v);
        if (d->value.string_v == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_set_value(): out of memory");
            return;
        }
    } else {
        d->value = *value;
    }


    d->dirty = 1;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
