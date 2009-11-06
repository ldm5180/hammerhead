
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>
#include <glib.h>
#include "libbdm-internal.h"

unsigned int bdm_cache_get_num_bdms(void)
{
    if (NULL == libbdm_bdms)
    {
	return 0;
    }

    return g_slist_length(libbdm_bdms);
} /* bdm_cache_get_num_bdms() */


bionet_bdm_t *bdm_cache_get_bdm_by_index(unsigned int index)
{
    if ((NULL == libbdm_bdms) || (index >= g_slist_length(libbdm_bdms)))
    {
	/* either there are no nodes or this is past the bounds of the list */
	return NULL;
    }
	
    return g_slist_nth_data(libbdm_bdms, index);
} /* bdm_cache_get_bdm_by_index() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
