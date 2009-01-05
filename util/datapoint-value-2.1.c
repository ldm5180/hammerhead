
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#define _GNU_SOURCE  // for strtof(3)

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "bionet-util-2.1.h"
#include "internal.h"



void bionet_datapoint_set_value(bionet_datapoint_t *d, bionet_value_t *value) {
    if (d == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_set_value(): NULL datapoint passed in");
        return;
    }
    if (value == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_set_value(): NULL value passed in");
        return;
    }

    
    if (value == d->value)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_datapoint_set_value(): passed in value == d->value");
	d->dirty = 1;
	return;
    }
    else if (d->value) {
        free(d->value);
	d->value = value;
    } else {
        d->value = value;
    }


    d->dirty = 1;
}


bionet_value_t * bionet_datapoint_get_value(bionet_datapoint_t *d)
{
    if (NULL == d)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_datapoint_get_value(): NULL datapoint passed in");
	errno = EINVAL;
	return NULL;
    }

    return d->value;
} /* bionet_datapoint_get_value() */

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
