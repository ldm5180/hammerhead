
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#ifndef WATCHER_CALLBACKS_H
#define WATCHER_CALLBACKS_H

#include "bionet-util.h"

typedef enum {
    OM_NORMAL,
    OM_TEST_PATTERN
} om_t;

void cb_datapoint(bionet_datapoint_t *datapoint);
void cb_lost_node(bionet_node_t *node);
void cb_new_node(bionet_node_t *node);
void cb_lost_hab(bionet_hab_t *hab);
void cb_new_hab(bionet_hab_t *hab);

void cb_bdm_datapoint(bionet_datapoint_t *datapoint, bionet_event_t * event, void * usr_data);
void cb_bdm_lost_node(bionet_node_t *node, bionet_event_t * event, void * usr_data);
void cb_bdm_new_node(bionet_node_t *node, bionet_event_t * event, void * usr_data);
void cb_bdm_lost_hab(bionet_hab_t *hab, bionet_event_t * event, void * usr_data);
void cb_bdm_new_hab(bionet_hab_t *hab, bionet_event_t * event, void * usr_data);

#endif /* WATCHER_CALLBACKS_H */
