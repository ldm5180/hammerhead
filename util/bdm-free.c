
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdlib.h>

#include <glib.h>

#include "internal.h"
#include "bionet-util.h"


static void bdm_destroy (gpointer data, gpointer user_data);


void bionet_bdm_free(bionet_bdm_t *bdm) {
    if (bdm == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_free(): NULL HAB passed in");
        return;
    }

    if(bdm->ref) {
	bdm->ref = bdm->ref - 1;
	return;
    }

    /* run all the destructors */
    g_slist_foreach(bdm->destructors,
		    bdm_destroy,
		    bdm);


    /* free all the destructors */
    while (bdm->destructors) {
	bionet_bdm_destructor_t * des = bdm->destructors->data;
	bdm->destructors = g_slist_remove(bdm->destructors, des);
	free(des);
    }

    if (bdm->user_data != NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_free(): passed-in BDM has non-NULL user_data pointer, ignoring");
    }

    if (bdm->id != NULL) {
        free(bdm->id);
    }

    g_ptr_array_free(bdm->hab_list, 1);

    free(bdm);
}


static void bdm_destroy (gpointer data, gpointer user_data) {
    bionet_bdm_destructor_t * des = (bionet_bdm_destructor_t *)data;
    bionet_bdm_t * bdm = (bionet_bdm_t *)user_data;

    if (NULL == des) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "bdm_destroy: NULL destructor passed in.");
	return;
    }

    if (NULL == bdm) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "bdm_destroy: NULL resource passed in.");
	return;
    }

    des->destructor(bdm, des->user_data);
} /* bdm_destroy() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
