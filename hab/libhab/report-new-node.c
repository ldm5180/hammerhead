
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <errno.h>
#include <string.h>

#include <glib.h>

#include "hardware-abstractor.h"

#include "libhab-internal.h"
#include "bionet-asn.h"


int hab_report_new_node(const bionet_node_t *node) {
    bionet_asn_buffer_t buf;
    int r;

    //
    // sanity checks
    //
    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_new_node(): NULL Node passed in");
        goto fail0;
    }

    bionet_hab_t * hab = bionet_node_get_hab(node);
    
    if (hab != libhab_this) {
	if (NULL == hab)
	{
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "hab_report_new_node(): passed-in Node does not belong to a HAB.");
	} else {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "hab_report_new_node(): passed-in Node does not belong to this HAB (it belongs to %s.%s, this hab is %s.%s)",
		  bionet_hab_get_type(hab),
		  bionet_hab_get_id(hab),
		  bionet_hab_get_type(libhab_this),
		  bionet_hab_get_id(libhab_this));
	}
        goto fail0;
    }

    if (bionet_hab_get_node_by_id(libhab_this, bionet_node_get_id(node)) != node) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "hab_report_new_node(): passed-in Node not found in this HAB");
        goto fail0;
    }


    // 
    // things look good, publish this node to node subscribers
    //

    r = bionet_node_to_asnbuf(node, &buf);
    if (r != 0) {
        // an error has already been logged
        goto fail1;
    }

    // publish the message to any connected subscribers
    cal_server.publish(bionet_node_get_id(node), buf.buf, buf.size);
    // FIXME: cal_server.publish should take the buf
    free(buf.buf);


    //
    // send this Node's Resources' metadata to datapoint subscribers
    // 

    {
        int ri;
        char resource_topic[(BIONET_NAME_COMPONENT_MAX_LEN * 2) + 2];

        for (ri = 0; ri < bionet_node_get_num_resources(node); ri ++) {
            bionet_resource_t *resource = bionet_node_get_resource_by_index(node, ri);
            int r;

            sprintf(resource_topic, "%s:%s", 
		    bionet_node_get_id(node), bionet_resource_get_id(resource));
            r = bionet_resource_metadata_to_asnbuf(resource, &buf);
            if (r != 0) {
                // an error has already been logged, and the buffer has been freed
                continue;
            }

            // publish the message to any connected subscribers
            cal_server.publish(resource_topic, buf.buf, buf.size);

            // FIXME: cal_server.publish should take the buf
            free(buf.buf);


            // send all datapoints
            r = bionet_resource_datapoints_to_asnbuf(resource, &buf, 0);
            if (r != 0) continue;

            bionet_resource_make_clean(resource);

            // publish the message to any connected subscribers
            cal_server.publish(resource_topic, buf.buf, buf.size);

            // FIXME: cal_server.publish should take the buf
            free(buf.buf);
        }
    }


    return 0;

fail1:
    if (buf.buf != NULL) free(buf.buf);

fail0:
    return -1;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
