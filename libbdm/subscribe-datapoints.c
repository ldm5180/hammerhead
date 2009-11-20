
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "bdm-client.h"
#include "libbdm-internal.h"


int bdm_subscribe_datapoints_by_bdmid_habtype_habid_nodeid_resourceid(
        const char *bdm_id,
        const char *hab_type,
        const char *hab_id,
        const char *node_id,
        const char *resource_id,
        struct timeval *datapoint_start,
        struct timeval *datapoint_end) 
{
    int r;
    char topic[(BIONET_NAME_COMPONENT_MAX_LEN * 5) + 6];  // the +6 is one for the leading "D " to specify the subscription family, and the separators
    libbdm_datapoint_subscription_t *new_datapoint_sub;

    new_datapoint_sub = calloc(1, sizeof(libbdm_datapoint_subscription_t));
    if (new_datapoint_sub == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s(): out of memory", __FUNCTION__);
        goto fail0;
    }

    new_datapoint_sub->hab_type = strdup(hab_type);
    if (new_datapoint_sub->hab_type == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s(): out of memory", __FUNCTION__);
        goto fail0;
    }

    new_datapoint_sub->hab_id = strdup(hab_id);
    if (new_datapoint_sub->hab_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s(): out of memory", __FUNCTION__);
        goto fail0;
    }

    new_datapoint_sub->node_id = strdup(node_id);
    if (new_datapoint_sub->node_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s(): out of memory", __FUNCTION__);
        goto fail0;
    }

    new_datapoint_sub->resource_id = strdup(resource_id);
    if (new_datapoint_sub->resource_id == NULL)  {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s(): out of memory", __FUNCTION__);
        goto fail0;
    }

    libbdm_datapoint_subscriptions = g_slist_prepend(libbdm_datapoint_subscriptions, new_datapoint_sub);

    if ( datapoint_start && datapoint_end ) {
        r = snprintf(topic, sizeof(topic), "D %s.%s.%s:%s?dpstart=%ld.%06ld&dpend=%ld.%06ld", 
                hab_type, hab_id, node_id, resource_id,
                (long)datapoint_start->tv_sec, (long)datapoint_start->tv_usec,
                (long)datapoint_end->tv_sec, (long)datapoint_end->tv_usec);
    } else if ( datapoint_start ) {
        r = snprintf(topic, sizeof(topic), "D %s.%s.%s:%s?dpstart=%ld.%06ld",
                hab_type, hab_id, node_id, resource_id,
                (long)datapoint_start->tv_sec, (long)datapoint_start->tv_usec);
    } else if ( datapoint_end ) {
        r = snprintf(topic, sizeof(topic), "D %s.%s.%s:%s?dpend=%ld.%06ld",
                hab_type, hab_id, node_id, resource_id,
                (long)datapoint_end->tv_sec, (long)datapoint_end->tv_usec);
    } else {
        r = snprintf(topic, sizeof(topic), "D %s.%s.%s:%s", 
                hab_type, hab_id, node_id, resource_id);
    }
    if (r >= sizeof(topic)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s(): topic '%s:%s' too long", __FUNCTION__, node_id, resource_id);
        goto fail0;
    }

    // send the subscription request to the HAB
    r = cal_client.subscribe(bdm_id, topic);
    if (!r) return -1;

    return 0;

fail0:
    free(new_datapoint_sub->resource_id);
    free(new_datapoint_sub->node_id);
    free(new_datapoint_sub->hab_id);
    free(new_datapoint_sub->hab_type);
    free(new_datapoint_sub);

    return -1;
}




int bdm_subscribe_datapoints_by_name(const char *resource_name) {
    char bdm_id[BIONET_NAME_COMPONENT_MAX_LEN+1];
    char hab_type[BIONET_NAME_COMPONENT_MAX_LEN+1];
    char hab_id[BIONET_NAME_COMPONENT_MAX_LEN+1];
    char node_id[BIONET_NAME_COMPONENT_MAX_LEN+1];
    char resource_id[BIONET_NAME_COMPONENT_MAX_LEN+1];
    int querystr, r;

    struct timeval tv_start;
    struct timeval *pDatapointStart = NULL;
    struct timeval tv_stop;
    struct timeval *pDatapointEnd = NULL;

    querystr = bdm_split_resource_name_r(resource_name, bdm_id, hab_type, hab_id, node_id, resource_id);
    if (querystr < 0) {
        // a helpful error message has already been logged
        return -1;
    }
    GHashTable * params = NULL;
    if(querystr) {
            r = bionet_parse_topic_params(resource_name + querystr, &params);
            if (r != 0) {
                    // Error logged
                    return -1;
            }

            if( 0 == bionet_param_to_timeval(params, "dpstart", &tv_start) ) {
                    pDatapointStart = &tv_start;
            }
            if( 0 == bionet_param_to_timeval(params, "dpend", &tv_stop) ) {
                    pDatapointEnd = &tv_stop;
            }
    }

    return bdm_subscribe_datapoints_by_bdmid_habtype_habid_nodeid_resourceid(
            bdm_id, hab_type, hab_id, node_id, resource_id,
            pDatapointStart, pDatapointEnd);

}
