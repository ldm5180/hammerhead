
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>
#include <glib.h>
#include "libbdm-internal.h"

extern GSList *bionet_habs;


unsigned int bdm_cache_get_num_habs(void)
{
    if (NULL == bionet_habs)
    {
	return 0;
    }

    return g_slist_length(bionet_habs);
} /* bdm_cache_get_num_habs() */


bionet_hab_t *bdm_cache_get_hab_by_index(unsigned int index)
{
    if ((NULL == bionet_habs) || (index >= g_slist_length(bionet_habs)))
    {
	/* either there are no nodes or this is past the bounds of the list */
	return NULL;
    }
	
    return g_slist_nth_data(bionet_habs, index);
} /* bdm_cache_get_hab_by_index() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
