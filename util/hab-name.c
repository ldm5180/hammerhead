
//
// Copyright (C) 2008, Regents of the University of Colorado.
//

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

const char *bionet_hab_get_name(bionet_hab_t *hab) {
    char buf[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    int r;

    if (NULL == hab) {
	errno = EINVAL;
	g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "bionet_hab_get_name(): NULL HAB passed in"
        );
	return NULL;
    }

    if (hab->name != NULL) return hab->name;

    r = snprintf(buf, sizeof(buf), "%s.%s", hab->type, hab->id);
    if (r >= sizeof(buf)) {
	errno = EINVAL;
	g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "bionet_hab_get_name(): HAB name %s.%s too long!",
            hab->type,
            hab->id
        );
	return NULL;
    }

    hab->name = strdup(buf);
    if (hab->name == NULL) {
	g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "bionet_hab_get_name(): out of memory!"
        );
	return NULL;
    }

    return hab->name;
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
