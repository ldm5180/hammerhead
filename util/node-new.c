
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "internal.h"
#include "bionet-util.h"



bionet_node_t* bionet_node_new(
        bionet_hab_t *hab,
	const char* node_id
) {
    bionet_node_t* node;

    if (node_id == NULL) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_new(): NULL Node-ID passed in");
        errno = EINVAL;
	return NULL;
    }

    if (!bionet_is_valid_name_component(node_id)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_new(): invalid Node-ID '%s' passed in", node_id);
        errno = EINVAL;
	return NULL;
    }

    node = calloc(1, sizeof(bionet_node_t));
    if (node == NULL) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "out of memory!");
	return NULL;
    }

    node->hab = hab;

    node->id = strdup(node_id);
    if (node->id == NULL) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "out of memory!");
        free(node);
	return NULL;
    }

    return node;
}


const char * bionet_node_get_id(const bionet_node_t *node)
{
    if (NULL == node)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_get_id(): NULL node passed in");
	errno = EINVAL;
	return NULL;
    }

    return node->id;
} /* bionet_node_get_id() */

const uint8_t * bionet_node_get_uid(const bionet_node_t *node)
{
    if (NULL == node)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_get_uid(): NULL node passed in");
	errno = EINVAL;
	return NULL;
    }

    return node->guid;
} /* bionet_node_get_id() */

void bionet_node_set_uid(bionet_node_t *node, const uint8_t uuid[BDM_UUID_LEN])
{
    if (NULL == node)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_set_uid(): NULL node passed in");
	errno = EINVAL;
	return;
    }

    memcpy(node->guid, uuid, BDM_UUID_LEN);
} /* bionet_node_set_uid() */

bionet_hab_t * bionet_node_get_hab(const bionet_node_t *node)
{
    if (NULL == node)
    {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_node_get_hab(): NULL node passed in");
	errno = EINVAL;
	return NULL;
    }

    return node->hab;
} /* bionet_node_get_hab() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
