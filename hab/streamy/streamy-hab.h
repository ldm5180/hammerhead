
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef __STREAMY_HAB_H
#define __STREAMY_HAB_H


#include "hardware-abstractor.h"


// this hab
extern bionet_hab_t *this_hab;

extern double bandwidth_limit;


void cb_stream_subscription(const char *client_id, const bionet_stream_t *stream);
void cb_stream_unsubscription(const char *client_id, const bionet_stream_t *stream);

void cb_stream_data(const char *client_id, bionet_stream_t *stream, const void *data, unsigned int size);

int read_from_bionet(GIOChannel *unused_channel, GIOCondition unused_condition, gpointer unused_data);

int publish_file(void *stream_as_voidp);


void make_incoming_node(bionet_hab_t *this_hab);
void make_outgoing_node(bionet_hab_t *this_hab);


#endif // __STREAMY_HAB_H

