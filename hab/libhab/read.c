
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include "cal-server.h"
#include "libhab-internal.h"
#include "hardware-abstractor.h"


void hab_read(void) {
    cal_server.read();
}

