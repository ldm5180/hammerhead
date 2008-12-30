
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <string.h>

#include <glib.h>

#include "bionet-util.h"
#include "bionet.h"

extern GSList *bionet_habs;

typedef struct {
    const char *hab_type;
    const char *hab_id;
} libbionet_hab_finder_t;


static int libbionet_find_hab_by_habtype_habid(const void *vp_hab, const void *vp_target) {
    const libbionet_hab_finder_t *target = vp_target;
    const bionet_hab_t *hab = vp_hab;

#ifdef BIONET_21_API
    if (strcmp(bionet_hab_get_type(hab), target->hab_type) != 0) {
        return -1;
    }

    if (strcmp(bionet_hab_get_id(hab), target->hab_id) != 0) {
        return -1;
    }
#else
    if (strcmp(hab->type, target->hab_type) != 0) {
        return -1;
    }

    if (strcmp(hab->id, target->hab_id) != 0) {
        return -1;
    }
#endif
    return 0;
}


bionet_hab_t *bionet_cache_lookup_hab(const char *hab_type, const char *hab_id) {
    GSList *p;

    libbionet_hab_finder_t target;


    if (hab_type == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_lookup_hab(): NULL hab_type passed in!");
        return NULL;
    }

    if (hab_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_lookup_hab(): NULL hab_id passed in!");
        return NULL;
    }

    target.hab_type = hab_type;
    target.hab_id   = hab_id;

    p = g_slist_find_custom(bionet_habs, &target, libbionet_find_hab_by_habtype_habid);
    if (p == NULL) {
        return NULL;
    }

    return (bionet_hab_t *)(p->data);
}


