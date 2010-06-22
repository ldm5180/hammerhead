#include <glib.h>

#include "hardware-abstractor.h"
#include "sim-hab.h"

int read_from_bionet(GIOChannel *unused_channel, GIOCondition unused_condition, gpointer unused_data) 
{
    hab_read_with_timeout(NULL);
    return 1;
}

