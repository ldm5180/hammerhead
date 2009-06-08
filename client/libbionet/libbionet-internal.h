
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


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

extern void (*libbionet_callback_stream)(bionet_stream_t *stream, void *buffer, int size);


extern int libbionet_cal_fd;


void libbionet_cal_callback(const cal_event_t *event);




// 
// This is the list of all HABs known to the Bionet Client library, which
// is the same as the list of CAL Servers known to the underlying CAL
// Client library.
//
// Compare to bionet_habs, from bionet.h
//

extern GSList *libbionet_habs;




// 
// these data structures track the client's registered subscriptions
//

typedef struct {
    char *hab_type;
    char *hab_id;
} libbionet_hab_subscription_t;

extern GSList *libbionet_hab_subscriptions;


typedef struct {
    char *hab_type;
    char *hab_id;
    char *node_id;
} libbionet_node_subscription_t;

extern GSList *libbionet_node_subscriptions;


typedef struct {
    char *hab_type;
    char *hab_id;
    char *node_id;
    char *resource_id;
} libbionet_datapoint_subscription_t;

extern GSList *libbionet_datapoint_subscriptions;
extern GSList *libbionet_stream_subscriptions;




//
// functions for dealing with the cache
//

void libbionet_cache_add_hab(bionet_hab_t *hab);
void libbionet_cache_remove_hab(bionet_hab_t *hab);

void libbionet_cache_add_node(bionet_node_t *node);
void libbionet_cache_remove_node(bionet_node_t *node);

void libbionet_cache_add_resource(bionet_resource_t *resource);
void libbionet_cache_replace_resource(bionet_resource_t *resource);

void libbionet_cache_cleanup_habs();
void libbionet_cache_cleanup_nodes();



#endif  // __LIBBIONET_INTERNAL_H

