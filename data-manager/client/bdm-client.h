
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef __BDM_H
#define __BDM_H


#include <stdint.h>

#include <glib.h>

#include "bionet-asn.h"

#include "bdm-util.h"




// FIXME: switch to CAL and this'll use mDNS-SD
int bdm_connect(char *hostname, uint16_t port);
int bdm_is_connected(void);
void bdm_disconnect(void);


GPtrArray *bdm_get_resource_datapoints(const char *resource_name_pattern, struct timeval *start, struct timeval *stop);


int bdm_send_asn(const void *buffer, size_t size, void *unused);




#endif

