
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#include "internal.h"
#include "bionet-util.h"

static int bionet_bdm_set_id(bionet_bdm_t *bdm, const char *id);


const char * bionet_bdm_get_id(const bionet_bdm_t *bdm) {
    if (bdm == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_bdm_get_type(): NULL BDM passed in!");
        return NULL;
    }

    return bdm->id;
}

static int bionet_bdm_set_id(bionet_bdm_t *bdm, const char *id) {

    // 
    // sanity checks
    //

    if (bdm == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_set_id(): NULL BDM passed in!");
        return -1;
    }

    if ((id != NULL) && !bionet_is_valid_name_component(id)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_bdm_set_id(): invalid BDM-ID passed in (%s)", id);
        return -1;
    }


    if (bdm->id != NULL) {
        free(bdm->id);
        bdm->id = NULL;
    }

    if (id != NULL) {
        bdm->id = strdup(id);
        if (bdm->id == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bionet_bdm_set_id(): out of memory!");
            return -1;
        }
    }

    if (NULL == id)
    {
        char hostname[256];
        char *p;
        int r;

        r = gethostname(hostname, sizeof(hostname));
        if (r < 0) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		  "bionet_bdm_set_id(): the passed-in BDM has no BDM-ID, and could not get hostname: %s", 
		  strerror(errno));
            return -1;
        }

        for (p = hostname; *p != '\0'; p++) {
            if (!isalnum(*p) && (*p != '-')) *p = '-';
        }

	bdm->id = strdup(hostname);
        if (bdm->id == NULL) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bionet_bdm_set_id(): out of memory!");
            return -1;
        }
    }

    return 0;
}


bionet_bdm_t* bionet_bdm_new(const char* id) {

    bionet_bdm_t* bdm;

    bdm = calloc(1, sizeof(bionet_bdm_t));
    if (bdm == NULL) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "bionet_bdm_new(): out of memory!");
	return NULL;
    }

    if (bionet_bdm_set_id(bdm, id) != 0) {
        // an error has been logged
        bionet_bdm_free(bdm);
        return NULL;
    }

    bdm->curr_seq = -1;

    return bdm;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
