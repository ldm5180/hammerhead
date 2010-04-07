
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "cal-mdnssd-bip.h"




int bip_peer_is_secure(bip_peer_t *peer) {
    bip_peer_network_info_t *net;

    if (peer == NULL) {
        g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bip_peer_is_secure: NULL peer passed in");
        return 0;
    }

    net = bip_peer_get_connected_net(peer);
    if (net == NULL) {
	return 0;
    }

    if (BIP_SEC_REQ == net->security_status) {
	return 1;
    }

    return 0;
}

