
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "test-pattern-hab.h"
#include "bionet.h"
#include "bionet-util.h"


void new_node(struct new_node_event_t *event, struct timeval *tv) {
    bionet_node_t *node;
    GSList *cursor;

    node = bionet_node_new(hab, event->id);
    bionet_hab_add_node(hab, node);

    for (cursor = event->resources; cursor != NULL; cursor = cursor->next) {
        struct resource_info_t* res_info;
        bionet_resource_t *resource;

        res_info = (struct resource_info_t*)cursor->data;

        resource = bionet_resource_new(node, res_info->data_type, res_info->flavor, res_info->id);
        if (res_info->has_value == TRUE) {
            bionet_value_t *value;
            bionet_datapoint_t *dp;

            value = str_to_value(resource, res_info->data_type, res_info->value);
            dp = bionet_datapoint_new(resource, value, tv);
            bionet_resource_add_datapoint(resource, dp);
        }

        bionet_node_add_resource(node, resource);
    }
    
    hab_report_new_node(node);
}



void update(struct datapoint_event_t *event, struct timeval *tv) {

    bionet_node_t *node;
    bionet_resource_t *resource;
    bionet_value_t *value;
    bionet_datapoint_t *dp;

    node = bionet_hab_get_node_by_id(hab, event->node_id);
    if (node == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "unable to update: node '%s' does not exist", event->node_id);
        return;
    }

    resource = bionet_node_get_resource_by_id(node, event->id);
    if (resource == NULL) {
        g_log("", G_LOG_LEVEL_WARNING, "unable to update: resource '%s' does not exist", event->id);
        return;
    }

    bionet_resource_remove_datapoint_by_index(resource, 0);
    value = str_to_value(resource, bionet_resource_get_data_type(resource), event->value);
    dp = bionet_datapoint_new(resource, value, tv);
    bionet_resource_add_datapoint(resource, dp);

    hab_report_datapoints(node);
}




void simulate_updates(gpointer data, gpointer user_data) {
    struct event_t *event = (struct event_t*)data;
    struct timeval **ptr;

    struct timeval *next, *last;
    int sec = 0, usec = 0;

    next = event->tv;

    ptr = (struct timeval**)user_data;
    if (*ptr == NULL) {
        last = next;
    } else {
        last = *ptr;
    }

    // determine the time to sleep
    sec = next->tv_sec - last->tv_sec;

    // milliseconds are a bit different ...
    if (sec == 0) {
        usec = next->tv_usec - last->tv_usec;
    } else {
        if (next->tv_usec >= last->tv_usec) {
            usec = next->tv_usec - last->tv_usec;
        } else {
            usec = 1000000 - (last->tv_usec - next->tv_usec);
            sec -= 1;
        }
    }

    usleep(usec);
    sleep(sec);

    switch (event->type) {
        case NEW_NODE: {
            new_node((struct new_node_event_t*)event->event, next);
            break;
        }
        case LOST_NODE: {
            struct lost_node_event_t* lost_event;

            lost_event = (struct lost_node_event_t*)event->event;
            bionet_hab_remove_node_by_id(hab, lost_event->id);
            hab_report_lost_node(lost_event->id);
            break;
        }
        case DATAPOINT_UPDATE: {
            update((struct datapoint_event_t*)event->event, next);
            break;
        }
    }

    // set the timeval for the next step
    *ptr = next;
}

