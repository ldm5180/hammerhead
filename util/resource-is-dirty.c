
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "internal.h"
#include "bionet-util.h"


int bionet_resource_is_dirty(const bionet_resource_t *resource) {
    int di;

    for (di = 0; di < bionet_resource_get_num_datapoints(resource); di ++) {
        bionet_datapoint_t *d;

        d = bionet_resource_get_datapoint_by_index(resource, di);
        if (bionet_datapoint_is_dirty(d)) return 1;
    }

    return 0;  // not dirty
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
