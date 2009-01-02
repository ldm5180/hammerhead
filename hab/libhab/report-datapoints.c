
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/time.h>

#include <glib.h>


#include "hardware-abstractor.h"

#include "libhab-internal.h"
#include "bionet-asn.h"




int hab_report_datapoints(const bionet_node_t *node) {
    int ri;

    for (ri = 0; ri < bionet_node_get_num_resources(node); ri ++) {
        bionet_asn_buffer_t buf;
        char topic[(BIONET_NAME_COMPONENT_MAX_LEN * 2) + 2];
        int r;
        bionet_resource_t *resource;

        resource = bionet_node_get_resource_by_index(node, ri);
        if (!bionet_resource_is_dirty(resource)) continue;

        // send dirty datapoints only
        r = bionet_resource_datapoints_to_asnbuf(resource, &buf, 1);
        if (r != 0) continue;

        bionet_resource_make_clean(resource);

#ifdef BIONET_21_API
        sprintf(topic, "%s:%s", bionet_node_get_id(node), bionet_resource_get_id(resource));
#else
        sprintf(topic, "%s:%s", node->id, resource->id);
#endif
        // publish the message to any connected subscribers
        cal_server.publish(topic, buf.buf, buf.size);

        // FIXME: cal_server.publish should take the buf
        free(buf.buf);
    }

    // FIXME: should be void maybe
    return 0;
}

