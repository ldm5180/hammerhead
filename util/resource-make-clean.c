
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#ifdef BIONET_21_API
#include "internal.h"
#include "bionet-util-2.1.h"
#else
#include "bionet-util.h"
#endif

void bionet_resource_make_clean(bionet_resource_t *resource) {
    int di;

    for (di = 0; di < bionet_resource_get_num_datapoints(resource); di ++) {
        bionet_datapoint_t *d;

        d = bionet_resource_get_datapoint_by_index(resource, di);
        bionet_datapoint_make_clean(d);
    }
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
