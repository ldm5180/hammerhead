
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#ifndef __LIBBIONET_INTERNAL_H
#define __LIBBIONET_INTERNAL_H


#include "cal-client.h"
#include "bionet.h"


extern char *libbionet_client_id;


extern void (*libbionet_callback_new_hab)(bionet_hab_t *hab);
extern void (*libbionet_callback_lost_hab)(bionet_hab_t *hab);

extern void (*libbionet_callback_new_node)(bionet_node_t *node);
extern void (*libbionet_callback_lost_node)(bionet_node_t *node);

extern void (*libbionet_callback_datapoint)(bionet_datapoint_t *datapoint);


extern int libbionet_cal_fd;


void libbionet_cal_callback(const cal_event_t *event);




//
// functions for dealing with the cache
//

void libbionet_cache_add_hab(bionet_hab_t *hab);
void libbionet_cache_remove_hab(bionet_hab_t *hab);

void libbionet_cache_add_node(bionet_node_t *node);
void libbionet_cache_remove_node(bionet_node_t *node);

void libbionet_cache_add_resource(bionet_resource_t *resource);
void libbionet_cache_replace_resource(bionet_resource_t *resource);




#endif  // __LIBBIONET_INTERNAL_H

