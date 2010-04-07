
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <string.h>

#include <glib.h>

#include "bionet-util.h"
#include "libbdm-internal.h"

static int libbdm_find_bdm_by_bdmid(const void *vp_bdm, const void *vp_target) {
    const char *target = vp_target;
    const bionet_bdm_t *bdm = vp_bdm;

    if (strcmp(bionet_bdm_get_id(bdm), target) != 0) {
        return -1;
    }

    return 0;
}


bionet_bdm_t *bdm_cache_lookup_bdm(const char *bdm_id) {
    GSList *p;

    const void * target;


    if (bdm_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_lookup_bdm(): NULL bdm_id passed in!");
        return NULL;
    }

    target = bdm_id;

    p = g_slist_find_custom(libbdm_bdms, target, libbdm_find_bdm_by_bdmid);
    if (p == NULL) {
        return NULL;
    }

    return (bionet_bdm_t *)(p->data);
}


