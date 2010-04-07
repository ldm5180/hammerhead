
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <glib.h>

#include "hardware-abstractor.h"

int read_from_bionet(GIOChannel *unused_channel, GIOCondition unused_condition, gpointer unused_data) {
    hab_read();
    return 1;
}

