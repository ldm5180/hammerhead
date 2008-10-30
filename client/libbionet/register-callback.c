
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


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

