
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "libbdm-internal.h"


void bdm_register_callback_new_bdm(void (*cb_new_bdm)(bionet_bdm_t *bdm, void* usr_data), void*usr_data) {
    libbdm_callback_new_bdm = cb_new_bdm;
    libbdm_callback_new_bdm_usr_data = usr_data;
}

void bdm_register_callback_lost_bdm(void (*cb_lost_bdm)(bionet_bdm_t *bdm, void* usr_data), void*usr_data) {
    libbdm_callback_lost_bdm = cb_lost_bdm;
    libbdm_callback_lost_bdm_usr_data = usr_data;
}

void bdm_register_callback_new_hab(void (*cb_new_hab)(bionet_hab_t *hab, bionet_event_t * event, void* usr_data), void*usr_data) {
    libbdm_callback_new_hab = cb_new_hab;
    libbdm_callback_new_hab_usr_data = usr_data;
}

void bdm_register_callback_lost_hab(void (*cb_lost_hab)(bionet_hab_t *hab, bionet_event_t * event, void* usr_data), void*usr_data) {
    libbdm_callback_lost_hab = cb_lost_hab;
    libbdm_callback_lost_hab_usr_data = usr_data;
}


void bdm_register_callback_new_node(void (*cb_new_node)(bionet_node_t *node, bionet_event_t * event, void* usr_data), void*usr_data) {
    libbdm_callback_new_node = cb_new_node;
    libbdm_callback_new_node_usr_data = usr_data;
}

void bdm_register_callback_lost_node(void (*cb_lost_node)(bionet_node_t *node, bionet_event_t * event, void* usr_data), void*usr_data) {
    libbdm_callback_lost_node = cb_lost_node;
    libbdm_callback_lost_node_usr_data = usr_data;
}


void bdm_register_callback_datapoint(void (*cb_datapoint)(bionet_datapoint_t *datapoint, bionet_event_t * event, void* usr_data), void*usr_data) {
    libbdm_callback_datapoint = cb_datapoint;
    libbdm_callback_datapoint_usr_data = usr_data;
}


void bdm_register_callback_stream(void (*cb_stream)(bionet_stream_t *stream, void *buffer, int size, void* usr_data), void*usr_data) {
    libbdm_callback_stream = cb_stream;
    libbdm_callback_stream_usr_data = usr_data;
}

