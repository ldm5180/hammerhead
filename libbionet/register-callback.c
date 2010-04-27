
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "libbionet-internal.h"


void bionet_register_callback_new_hab(void (*cb_new_hab)(bionet_hab_t *hab)) {
    libbionet_callback_new_hab = cb_new_hab;
}

void bionet_register_callback_lost_hab(void (*cb_lost_hab)(bionet_hab_t *hab)) {
    libbionet_callback_lost_hab = cb_lost_hab;
}


void bionet_register_callback_new_node(void (*cb_new_node)(bionet_node_t *node)) {
    libbionet_callback_new_node = cb_new_node;
}

void bionet_register_callback_lost_node(void (*cb_lost_node)(bionet_node_t *node)) {
    libbionet_callback_lost_node = cb_lost_node;
}


void bionet_register_callback_datapoint(void (*cb_datapoint)(bionet_datapoint_t *datapoint)) {
    libbionet_callback_datapoint = cb_datapoint;
}


void bionet_register_callback_stream(void (*cb_stream)(bionet_stream_t *stream, void *buffer, int size)) {
    libbionet_callback_stream = cb_stream;
}

void bionet_register_callback_idle(void (*cb_idle)(void)) {
    libbionet_callback_idle = cb_idle;
}
