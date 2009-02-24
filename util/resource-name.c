
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <glib.h>

#include "bionet-util.h"
#include "internal.h"


const char *bionet_resource_get_name(const bionet_resource_t * resource) {
    char buf[4 * BIONET_NAME_COMPONENT_MAX_LEN];
    int r;

    if (resource == NULL) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_get_name(): NULL Resource passed in");
	errno = EINVAL;
	return NULL;
    }

    if (resource->id == NULL) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_get_name(): passed-in Resource has NULL ID");
	errno = EINVAL;
	return NULL;
    }

    if (resource->node == NULL) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_get_name(): passed-in Resource has NULL Node");
	errno = EINVAL;
	return NULL;
    }

    if (resource->node->id == NULL) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_get_name(): passed-in Resource has Node with NULL ID");
	errno = EINVAL;
	return NULL;
    }

    if (resource->node->hab == NULL) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_get_name(): passed-in Resource has Node with NULL HAB");
	errno = EINVAL;
	return NULL;
    }

    if (resource->node->hab->type == NULL) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_get_name(): passed-in Resource has Node with HAB with NULL Type");
	errno = EINVAL;
	return NULL;
    }

    if (resource->node->hab->id == NULL) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_get_name(): passed-in Resource has Node with HAB with NULL ID");
	errno = EINVAL;
	return NULL;
    }

    if (resource->name != NULL) return resource->name;

    r = snprintf(
        buf,
        sizeof(buf),
        "%s.%s.%s:%s",
        resource->node->hab->type,
        resource->node->hab->id,
        resource->node->id,
        resource->id
    );

    if (r >= sizeof(buf)) {
	errno = EINVAL;
	g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "bionet_resource_get_name(): Resource name %s.%s.%s:%s too long!",
            resource->node->hab->type,
            resource->node->hab->id,
            resource->node->id,
            resource->id
        );
	return NULL;
    }

    ((bionet_resource_t *)resource)->name = strdup(buf);
    if (resource->name == NULL) {
	g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "bionet_resource_get_name(): out of memory!"
        );
	return NULL;
    }

    return resource->name;
}


const char *bionet_resource_get_local_name(const bionet_resource_t * resource) {
    char buf[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    int r;

    if (resource == NULL) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_get_local_name(): NULL Resource passed in");
	errno = EINVAL;
	return NULL;
    }

    if (resource->id == NULL) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_get_local_name(): passed-in Resource has NULL ID");
	errno = EINVAL;
	return NULL;
    }

    if (resource->node == NULL) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_get_local_name(): passed-in Resource has NULL Node");
	errno = EINVAL;
	return NULL;
    }

    if (resource->node->id == NULL) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
	      "bionet_resource_get_local_name(): passed-in Resource has Node with NULL ID");
	errno = EINVAL;
	return NULL;
    }

    if (resource->local_name != NULL) return resource->local_name;

    r = snprintf(
        buf,
        sizeof(buf),
        "%s:%s",
        resource->node->id,
        resource->id
    );

    if (r >= sizeof(buf)) {
	errno = EINVAL;
	g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "bionet_resource_get_local_name(): local Resource name %s:%s too long!",
            resource->node->id,
            resource->id
        );
	return NULL;
    }

    ((bionet_resource_t *)resource)->local_name = strdup(buf);
    if (resource->local_name == NULL) {
	g_log(
            BIONET_LOG_DOMAIN,
            G_LOG_LEVEL_WARNING,
            "bionet_resource_get_local_name(): out of memory!"
        );
	return NULL;
    }

    return resource->local_name;
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
