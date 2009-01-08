
//
// Copyright (C) 2008, Regents of the University of Colorado.
// 


#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "internal.h"
#include "bionet-util.h"


const char *bionet_node_get_name(const bionet_node_t * node) {
    char buf[3 * BIONET_NAME_COMPONENT_MAX_LEN];
    int r;

    if (NULL == node) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_get_name(): NULL Node passed in");
	errno = EINVAL;
	return NULL;
    }

    if (NULL == node->hab) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_get_name(): passed-in Node has no HAB!");
	errno = EINVAL;
	return NULL;
    }

    if (node->name != NULL) return node->name;

    r = snprintf(buf, sizeof(buf), "%s.%s.%s", node->hab->type, node->hab->id, node->id);
    if (r >= sizeof(buf)) {
	errno = EINVAL;
	g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "bionet_node_get_name(): Node name %s.%s.%s too long!",
            node->hab->type,
            node->hab->id,
            node->id
        );
	return NULL;
    }

    ((bionet_node_t *)node)->name = strdup(buf);
    if (node->name == NULL) {
	g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "bionet_node_get_name(): out of memory!"
        );
	return NULL;
    }

    return node->name;
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
