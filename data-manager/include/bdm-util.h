
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef BDM_UTIL_H
#define BDM_UTIL_H


#include <stdint.h>
#include <time.h>

#include <sys/time.h>

#include <glib.h>

#include "bionet-util.h"



//
// The bionet code uses glib's logging facility to log internal messages,
// and this is the domain.
//

#define  BDM_LOG_DOMAIN  "bdm"

#define BDM_TOPIC_MAX_LEN (4*BIONET_NAME_COMPONENT_MAX_LEN + 4)



//
// bdm server listens on this TCP port, clients connect
//

#define BDM_PORT      (11002)
#define BDM_DEFAULT_HOST ("localhost")
#define BDM_SYNC_PORT (11003)



#endif //  BDM_UTIL_H

