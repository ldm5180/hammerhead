
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#ifndef BDM_UTIL_H
#define BDM_UTIL_H


#include <stdint.h>
#include <time.h>

#include <sys/time.h>

#include <glib.h>




//
// The bionet code uses glib's logging facility to log internal messages,
// and this is the domain.
//

#define  BDM_LOG_DOMAIN  "bdm"




//
// bdm server listens on this TCP port, clients connect
//

#define BDM_PORT (11002)




#endif //  BDM_UTIL_H

