
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "cal-client-mdnssd-bip.h"


char *cal_client_mdnssd_bip_network_type = NULL;

void (*cal_client_mdnssd_bip_callback)(cal_event_t *event) = NULL;

int cal_client_mdnssd_bip_fds_to_user[2] = { -1, -1 };
int cal_client_mdnssd_bip_fds_from_user[2] = { -1, -1 };

pthread_t *cal_client_mdnssd_bip_thread = NULL;

