#include <glib.h>
#include <stdlib.h>
#include <stdio.h>

#include "test-pattern-hab.h"


void free_new_node(struct new_node_event_t *node_event) {
    GSList *cursor;

    cursor = node_event->resources;
    while (cursor != NULL) {
        struct resource_info_t *resource = (struct resource_info_t*)cursor->data;
       
        if (resource->has_value)
            free((char*)resource->value);
        free(resource);
        resource = NULL;

        cursor = g_slist_delete_link(cursor, cursor);
    }

    free(node_event);
}


void free_update(struct datapoint_event_t *update_event) {
    free((char*)update_event->value);
    free(update_event);
}


void free_lost_node(struct lost_node_event_t *lost_node) {
    free(lost_node);
}


void free_event(struct event_t *event) {
    switch(event->type) {
        case NEW_NODE:
            free_new_node((struct new_node_event_t*)event->event);
            break;
        case DATAPOINT_UPDATE:
            free_update((struct datapoint_event_t*)event->event);
            break;
        case LOST_NODE:
            free_lost_node((struct lost_node_event_t*)event->event);
            break;
        default:
            break;
    }

    free(event->tv);
    free(event);
}
