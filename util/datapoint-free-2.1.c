
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdlib.h>

#include <glib.h>

#include "bionet-util.h"


void bionet_datapoint_free(bionet_datapoint_t *d) {
    if (d->resource->data_type == BIONET_RESOURCE_DATA_TYPE_STRING) {
        if (d->value.string_v != NULL) {
            free(d->value.string_v);
        }
    }
    free(d);
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
