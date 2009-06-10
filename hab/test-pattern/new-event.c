
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


#include "test-pattern-hab.h"

struct event_t *add_node_event(struct timeval *tv, char *id, GSList *resources) {
    struct event_t *event;
    struct new_node_event_t *node;

    node = calloc(1, sizeof(struct new_node_event_t));
    if (node == NULL) {
        g_log("", G_LOG_LEVEL_ERROR, "calloc failed (unable to create new_node_event): %s\n", strerror(errno));
        exit(1);
    }

    strncpy(node->id, id, BIONET_NAME_COMPONENT_MAX_LEN);
    if (BIONET_NAME_COMPONENT_MAX_LEN > 0)
        node->id[BIONET_NAME_COMPONENT_MAX_LEN - 1] = '\0';
    node->resources = resources;

    event = calloc(1, sizeof(struct event_t));
    if (event == NULL) {
        g_log("", G_LOG_LEVEL_ERROR, "calloc failed (unable to create new event): %s\n", strerror(errno));
        exit(1);
    }

    event->tv = tv;
    event->type = NEW_NODE;
    event->event = (void*)node;

    free(id);

    return event;
}


struct event_t *remove_node_event(struct timeval *tv, char *id) {
    struct event_t *event;
    struct lost_node_event_t *node;

    node = calloc(1, sizeof(struct lost_node_event_t));
    if (node == NULL) {
        g_log("", G_LOG_LEVEL_ERROR, "calloc failed (unable to create lost_node_event): %s", strerror(errno));
        exit(1);
    }

    strncpy(node->id, id, BIONET_NAME_COMPONENT_MAX_LEN);
    if (BIONET_NAME_COMPONENT_MAX_LEN > 0)
        node->id[BIONET_NAME_COMPONENT_MAX_LEN - 1] = '\0';

    event = calloc(1, sizeof(struct event_t));
    if (event == NULL) {
        g_log("", G_LOG_LEVEL_ERROR, "calloc failed (unable to create lost event): %s", strerror(errno));
        exit(1);
    }

    event->tv = tv;
    event->type = LOST_NODE;
    event->event = (void*)node;

    free(id);

    return event;
}


struct event_t *update_event(struct timeval *tv, char *node, char *resource, char *value) {
    struct event_t *event;
    struct datapoint_event_t *dp;

    dp = calloc(1, sizeof(struct datapoint_event_t));
    if (dp == NULL) {
        g_log("", G_LOG_LEVEL_ERROR, "calloc failed (unable to create update datapoint event): %s", strerror(errno));
        exit(1);
    }

    strncpy(dp->id, resource, BIONET_NAME_COMPONENT_MAX_LEN);
    strncpy(dp->node_id, node, BIONET_NAME_COMPONENT_MAX_LEN);
    if (BIONET_NAME_COMPONENT_MAX_LEN > 0) {
        dp->id[BIONET_NAME_COMPONENT_MAX_LEN - 1] = '\0';
        dp->node_id[BIONET_NAME_COMPONENT_MAX_LEN - 1] = '\0';
    }
    dp->value = value;

    event = calloc(1, sizeof(struct event_t));
    if (event == NULL) {
        g_log("", G_LOG_LEVEL_ERROR, "calloc failed (unable to create update event): %s", strerror(errno));
        exit(1);
    }

    event->tv = tv;
    event->type = DATAPOINT_UPDATE;
    event->event = (void*)dp;

    free(node);
    free(resource);

    return event;
}

