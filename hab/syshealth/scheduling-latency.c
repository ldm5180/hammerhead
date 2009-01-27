
//
// Copyright (C) 2008-2009, the Regents of the University of Colorado.
//


#include <errno.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include <glib.h>
#include <hardware-abstractor.h>

#include "syshealth.h"




static int scheduling_latency_get_one(uint32_t *microseconds) {
    struct timeval before, after;

    if (gettimeofday(&before, NULL) != 0) {
        g_log("", G_LOG_LEVEL_WARNING, "scheduling-latency: error getting reading: %s", strerror(errno));
        return -1;
    }

    sched_yield();

    if (gettimeofday(&after, NULL) != 0) {
        g_log("", G_LOG_LEVEL_WARNING, "scheduling-latency: error getting reading: %s", strerror(errno));
        return -1;
    }

    *microseconds = ((after.tv_sec - before.tv_sec) * 1000000) + (after.tv_usec - before.tv_usec);
    return 0;
}




#define SCHEDULING_LATENCY_READINGS_TO_AVERAGE (10)

static int scheduling_latency_get(uint32_t *microseconds) {
    int i;
    uint32_t val;
    uint32_t avg;

    avg = 0;
    for (i = 0; i < SCHEDULING_LATENCY_READINGS_TO_AVERAGE; i ++) {
        int r;
        r = scheduling_latency_get_one(&val);
        if (r < 0) return -1;
        avg += val;
    }
    avg /= SCHEDULING_LATENCY_READINGS_TO_AVERAGE;

    *microseconds = avg;

    return 0;
}




int scheduling_latency_init(bionet_node_t *node) {
    int r;
    uint32_t val;
    bionet_resource_t *resource;

    r = scheduling_latency_get(&val);
    if (r < 0) return -1;

    resource = bionet_resource_new(node,
            BIONET_RESOURCE_DATA_TYPE_UINT32,
            BIONET_RESOURCE_FLAVOR_SENSOR, 
            "Scheduling-Latency-us");
    if (resource == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "scheduling_latency_init(): error creating resource Scheduling-Latency-us");
        return -1;
    }

    r = bionet_resource_set_uint32(resource, val, NULL);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "scheduling_latency_init(): error setting resource");
        return -1;
    }

    r = bionet_node_add_resource(node, resource);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "scheduling_latency_init(): node unable to add resource Scheduling-Latency-us");
        return r;
    }

    return 0;
}


void scheduling_latency_update(bionet_node_t *node) {
    int r;
    uint32_t val;
    bionet_resource_t *resource;

    r = scheduling_latency_get(&val);
    if (r < 0) return;

    resource = bionet_node_get_resource_by_id(node, "Scheduling-Latency-us");
    if (resource == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "scheduling_latency_update(): couldn't find resource Scheduling-Latency-us");
        return;
    }

    r = bionet_resource_set_uint32(resource, val, NULL);
    if (r < 0) {
        g_log("", G_LOG_LEVEL_WARNING, "scheduling_latency_update(): error setting resource value");
    }
}

