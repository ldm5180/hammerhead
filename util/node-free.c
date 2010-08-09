
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>

#include <glib.h>

#include "internal.h"
#include "bionet-util.h"


void bionet_node_free(bionet_node_t *node) {
    if (node == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "NULL Node passed to bionet_node_free()!");
        return;
    }

    if (node->user_data != NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_free_node(): Node '%s' has non-NULL user_data pointer, ignoring", node->id);
    }

    bionet_hab_t * hab = bionet_node_get_hab(node);
    if (hab && bionet_hab_get_node_by_id(hab, bionet_node_get_id(node))) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_free_node(): Node is still in %s HAB list. It should have been removed before calling bionet_free_node().",
	      bionet_hab_get_name(hab));
    }

    if (node->id != NULL) {
        free(node->id);
    }

    if (node->name != NULL) {
        free(node->name);
    }


    // free all the resources
    while (node->resources != NULL) {
        bionet_resource_t *resource = node->resources->data;

        node->resources = g_slist_remove(node->resources, resource);
        bionet_resource_free(resource);
    }


    // free all the streams
    while (node->streams != NULL) {
        bionet_stream_t *stream = node->streams->data;

        node->streams = g_slist_remove(node->streams, stream);
        bionet_stream_free(stream);
    }


    // free all the events
    while (node->events != NULL) {
        bionet_event_t *event = node->events->data;

        node->events = g_slist_remove(node->events, event);
        bionet_event_free(event);
    }

    free(node);
}


