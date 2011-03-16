
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "bdm-client.h"
#include "libbdm-internal.h"


int bdm_unsubscribe_datapoints_by_habtype_habid_nodeid_resourceid(const char *peer_id,
								  const char *bdm_id,
								  const char *hab_type,
								  const char *hab_id, 
								  const char *node_id, 
								  const char *resource_id,
								  struct timeval *datapoint_start,
								  struct timeval *datapoint_end) {
    int r;
    char topic[(BIONET_NAME_COMPONENT_MAX_LEN * 2) + 2];  // the +2 is one for the leading "D " to specify the subscription family
    GSList *i = libbdm_datapoint_subscriptions;

    while (i != NULL) {
        libbdm_datapoint_subscription_t *dp_sub = i->data;

        if (dp_sub == NULL) {
	  g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		"%s: encountered a NULL subscription", __FUNCTION__);
            i = i->next;
            continue;
        }

        if ((dp_sub->hab_type == NULL) || (dp_sub->hab_id == NULL) || 
            (dp_sub->node_id == NULL) || (dp_sub->node_id == NULL)) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "%s: encountered a NULL subscription component", __FUNCTION__);
            i = i->next;
            continue;
        }

        if (!((strcmp(dp_sub->hab_type, hab_type) == 0) &&
            (strcmp(dp_sub->hab_id, hab_id) == 0) &&
            (strcmp(dp_sub->node_id, node_id) == 0) &&
            (strcmp(dp_sub->resource_id, resource_id) == 0))) {
            i = i->next;
            continue;
        }

        free(dp_sub->hab_type);
        free(dp_sub->hab_id);
        free(dp_sub->node_id);
        free(dp_sub->resource_id);
        free(dp_sub);
        i->data = NULL;
        
        libbdm_datapoint_subscriptions = g_slist_delete_link(libbdm_datapoint_subscriptions, i);

        libbdm_cache_cleanup_nodes();

	if ( datapoint_start && datapoint_end ) {
	    r = snprintf(topic, sizeof(topic), "D %s/%s.%s.%s:%s?dpstart=%ld.%06ld&dpend=%ld.%06ld", 
			 bdm_id, hab_type, hab_id, node_id, resource_id,
			 (long)datapoint_start->tv_sec, (long)datapoint_start->tv_usec,
			 (long)datapoint_end->tv_sec, (long)datapoint_end->tv_usec);
	} else if ( datapoint_start ) {
	    r = snprintf(topic, sizeof(topic), "D %s/%s.%s.%s:%s?dpstart=%ld.%06ld",
			 bdm_id, hab_type, hab_id, node_id, resource_id,
			 (long)datapoint_start->tv_sec, (long)datapoint_start->tv_usec);
	} else if ( datapoint_end ) {
	    r = snprintf(topic, sizeof(topic), "D %s/%s.%s.%s:%s?dpend=%ld.%06ld",
			 bdm_id, hab_type, hab_id, node_id, resource_id,
			 (long)datapoint_end->tv_sec, (long)datapoint_end->tv_usec);
	} else {
	    r = snprintf(topic, sizeof(topic), "D %s/%s.%s.%s:%s", 
			 bdm_id, hab_type, hab_id, node_id, resource_id);
	}

        if (r >= sizeof(topic)) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "%s: topic '%s:%s' too long", 
		  __FUNCTION__, node_id, resource_id);
            return -1;
        }

        // send the subscription request to the HAB
        r = cal_client.unsubscribe(libbdm_cal_handle, peer_id, topic);
        if (!r) return -1;

        return 0;
    }

    errno = ENOENT;
    return -1;
} /* bdm_unsubscribe_datapoints_by_habtype_habid_nodeid_resourceid() */


int bdm_unsubscribe_datapoints_by_name(const char *resource_name, 
				       struct timeval *datapoint_start,
				       struct timeval *datapoint_end) {
    char peer_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char bdm_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
    char hab_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char node_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];
    int r;

    r = bdm_split_resource_name_r(resource_name, peer_id, bdm_id, hab_type, hab_id, node_id, resource_id);
    if (r != 0) {
        // a helpful error message has already been logged
        return -1;
    }

    return bdm_unsubscribe_datapoints_by_habtype_habid_nodeid_resourceid(peer_id, bdm_id, 
									 hab_type, hab_id, 
									 node_id, resource_id,
									 datapoint_start, datapoint_end);
} /* bdm_unsubscribe_datapoints_by_name() */

