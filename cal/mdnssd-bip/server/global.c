
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "cal-server-mdnssd-bip.h"


char *cal_server_mdnssd_bip_network_type = NULL;

void (*cal_server_mdnssd_bip_callback)(cal_event_t *event) = NULL;

// pipes between CAL Server thread and user thread
bip_msg_queue_t bip_server_msgq;

