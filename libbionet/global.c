
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>


#include "libbionet-internal.h"
#include "bionet.h"




char *libbionet_client_id = NULL;


void (*libbionet_callback_new_hab)(bionet_hab_t *hab) = NULL;
void (*libbionet_callback_lost_hab)(bionet_hab_t *hab) = NULL;

void (*libbionet_callback_new_node)(bionet_node_t *node) = NULL;
void (*libbionet_callback_lost_node)(bionet_node_t *node) = NULL;

void (*libbionet_callback_datapoint)(bionet_datapoint_t *datapoint) = NULL;

void (*libbionet_callback_stream)(bionet_stream_t *stream, void *buffer, int size) = NULL;

void (*libbionet_callback_idle)(void) = NULL;


void * libbionet_cal_handle = NULL;


GSList *bionet_habs = NULL;
GSList *libbionet_habs = NULL;

GSList *libbionet_hab_subscriptions = NULL;
GSList *libbionet_node_subscriptions = NULL;
GSList *libbionet_datapoint_subscriptions = NULL;
GSList *libbionet_stream_subscriptions = NULL;


#if 0
// each item on this list is a bionet_message_t *
GSList *libbionet_queued_messages_from_nag = NULL;


char *libbionet_nag_hostname = NULL;
unsigned short libbionet_nag_port = 11000;

int libbionet_nag_timeout = 5;

char *libbionet_nag_error = NULL;

#endif


