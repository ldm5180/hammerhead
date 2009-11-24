
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "libbdm-internal.h"

void (*libbdm_callback_new_bdm)(bionet_bdm_t *bdm, void* usr_data) = NULL;
void * libbdm_callback_new_bdm_usr_data = NULL;
void (*libbdm_callback_lost_bdm)(bionet_bdm_t *bdm, void* usr_data) = NULL;
void * libbdm_callback_lost_bdm_usr_data = NULL;

void (*libbdm_callback_new_hab)(bionet_hab_t *hab, void* usr_data) = NULL;
void * libbdm_callback_new_hab_usr_data = NULL;
void (*libbdm_callback_lost_hab)(bionet_hab_t *hab, void* usr_data) = NULL;
void * libbdm_callback_lost_hab_usr_data = NULL;

void (*libbdm_callback_new_node)(bionet_node_t *node, void* usr_data) = NULL;
void * libbdm_callback_new_node_usr_data = NULL;
void (*libbdm_callback_lost_node)(bionet_node_t *node, void* usr_data) = NULL;
void * libbdm_callback_lost_node_usr_data = NULL;

void (*libbdm_callback_datapoint)(bionet_datapoint_t *datapoint, void* usr_data) = NULL;
void * libbdm_callback_datapoint_usr_data = NULL;

void (*libbdm_callback_stream)(bionet_stream_t *stream, void *buffer, int size, void* usr_data) = NULL;
void * libbdm_callback_stream_usr_data = NULL;


int libbdm_cal_fd = -1;

// THIS IS NOT USED.
// Old clients will assign the result of bdm_connect(host,port) to this.
// The global variable is here to maintain backword library compatibility
int bdm_fd = -1;

GSList *libbdm_bdms = NULL;
GHashTable *libbdm_all_peers = NULL;
GSList *libbdm_habs = NULL;

GSList *libbdm_bdm_subscriptions = NULL;
GSList *libbdm_hab_subscriptions = NULL;
GSList *libbdm_node_subscriptions = NULL;
GSList *libbdm_datapoint_subscriptions = NULL;
GSList *libbdm_stream_subscriptions = NULL;
