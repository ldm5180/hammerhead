
//
// Copyright (C) 2008-2009, Gary Grobe and the Regents of the University of Colorado.
//


#include <glib.h>

#include "hardware-abstractor.h"

int read_from_bionet(GIOChannel *unused_channel, GIOCondition unused_condition, gpointer unused_data) {
    hab_read();
    return 1;
}

