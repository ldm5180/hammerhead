
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "bdm-client.h"

bionet_hab_t * bionet_get_hab(GPtrArray *hab_list, int index)
{
    if (hab_list->len <= index)
    {
	return NULL;
    }

    return g_ptr_array_index(hab_list, index);
} /* bdm_get_hab() */


// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
