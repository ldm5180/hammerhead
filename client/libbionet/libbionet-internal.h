
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


#endif  // __LIBBIONET_INTERNAL_H

