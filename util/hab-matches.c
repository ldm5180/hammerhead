
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <string.h>

#include "bionet-util.h"


int bionet_hab_matches_type_and_id(const bionet_hab_t *hab, const char *type, const char *id) {
    if (
        (
            (strcmp(type, "*") == 0) ||
            (strcmp(type, hab->type) == 0) 
        ) &&
        (
            (strcmp(id, "*") == 0) ||
            (strcmp(id, hab->id) == 0) 
        )
    ) {
        return 1;
    }

    return 0;
}


