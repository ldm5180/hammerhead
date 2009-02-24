
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef SYSHEALTH_H
#define SYSHEALTH_H


#include <hardware-abstractor.h>




int disk_free_init(bionet_node_t *node);
void disk_free_update(bionet_node_t *node);

int mem_free_init(bionet_node_t *node);
void mem_free_update(bionet_node_t *node);

int uptime_init(bionet_node_t *node);
void uptime_update(bionet_node_t *node);

int load_average_init(bionet_node_t *node);
void load_average_update(bionet_node_t *node);

int smart_init(bionet_node_t *node);
void smart_update(bionet_node_t *node);

int sensors_init(bionet_node_t *node);
void sensors_update(bionet_node_t *node);

int scheduling_latency_init(bionet_node_t *node);
void scheduling_latency_update(bionet_node_t *node);


#endif

