
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/time.h>

#include <glib.h>

#include "bionet-util.h"
#include "hardware-abstractor.h"

#include "libhab-internal.h"
#include "bionet-asn.h"
#include "protected.h"

extern char * persist_dir;

int hab_report_datapoints(const bionet_node_t *node) {
    int ri;

    for (ri = 0; ri < bionet_node_get_num_resources(node); ri ++) {
        bionet_asn_buffer_t buf;
        int r;
        char topic[(BIONET_NAME_COMPONENT_MAX_LEN * 2) + 2];
        bionet_resource_t *resource;

        resource = bionet_node_get_resource_by_index(node, ri);
        if (!bionet_resource_is_dirty(resource)) continue;

	/* persist the resource if necessary */
	if (bionet_resource_is_persisted(resource)) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
		  "hab_report_datapoints: Persisting resource %s", 
		  bionet_resource_get_name(resource));
	    if (bionet_resource_write_persist(resource, persist_dir)) {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		      "hab_report_datapoints: Failed to persist resource %s",
		      bionet_resource_get_name(resource));
	    }
	}

	/* do delta/epsilon checking */
	bionet_epsilon_t * epsilon = bionet_resource_get_epsilon(resource);
	const struct timeval * delta = bionet_resource_get_delta(resource);
	bionet_datapoint_t * dp = NULL;
	bionet_datapoint_t * new_dp = NULL;

	if (delta || epsilon) {
	    dp = g_hash_table_lookup(libhab_most_recently_published, resource);
	    if (NULL == dp) {
		goto publish;
	    }
	} else {
	    goto publish;
	}

	if (epsilon) {
	    bionet_value_t * recent_val = bionet_datapoint_get_value(dp);
	    if (NULL == recent_val) {
		goto publish;
	    }

	    if (bionet_value_check_epsilon_by_value(recent_val,
						    bionet_datapoint_get_value(BIONET_RESOURCE_GET_DATAPOINT(resource)), 
						    epsilon, 
						    bionet_resource_get_data_type(resource))) {
		g_warning("Epsilon triggered");
		goto publish;
	    }
	}

	if (delta) {
	    struct timeval * recent_tv = bionet_datapoint_get_timestamp(dp);

	    if (NULL == recent_tv) {
		goto publish;
	    }

	    if (recent_tv && bionet_value_check_delta(recent_tv,
					 bionet_datapoint_get_timestamp(BIONET_RESOURCE_GET_DATAPOINT(resource)), 
					 delta)) {
		g_warning("Delta triggered");
		goto publish;
	    }
	}

	/* Delta/Epsilon didn't trigger. This resource doesn't need to be published. */
	bionet_resource_make_clean(resource);
	continue;

publish:
	/* if there is a delta or an epsilon, put a copy of the datapoint in the hash table */
	new_dp = BIONET_RESOURCE_GET_DATAPOINT(resource);
	if (new_dp) {
	    bionet_datapoint_t * recent_dp = bionet_datapoint_dup(new_dp);
	    if (recent_dp) {
		bionet_pthread_mutex_lock(&published_hash_mutex);
		g_hash_table_insert(libhab_most_recently_published, resource, recent_dp);
		bionet_pthread_mutex_unlock(&published_hash_mutex);	
	    } else {
		g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Failed to dup a datapoint.");
		return 1;
	    }
	}

        // send dirty datapoints only
        r = bionet_resource_datapoints_to_asnbuf(resource, &buf, 1, libhab_most_recently_published, &published_hash_mutex);
        if (r != 0) continue;
	
        bionet_resource_make_clean(resource);

        snprintf(topic, sizeof(topic), 
		 "D %s", bionet_resource_get_local_name(resource));

        // publish the message to any connected subscribers
        cal_server.publish(libhab_cal_handle, topic, buf.buf, buf.size);

        // FIXME: cal_server.publish should take the buf
        free(buf.buf);
    }

    // FIXME: should be void maybe
    return 0;
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
