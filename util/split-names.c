
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


// 
// The functions in this file take strings containing the names of various
// Bionet objects and split the strings into the components of the names.
//

#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "internal.h"
#include "bionet-util.h"


// this function does not mind tainted data, it defensively sanity-checks what you pass in
// coverity[ -tainted_data_sink : arg-0 ]
int bionet_split_resource_name(
    const char *resource_name,
    char **hab_type,
    char **hab_id,
    char **node_id,
    char **resource_id
) {
    static char internal_hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
    static char internal_hab_id[BIONET_NAME_COMPONENT_MAX_LEN];
    static char internal_node_id[BIONET_NAME_COMPONENT_MAX_LEN];
    static char internal_resource_id[BIONET_NAME_COMPONENT_MAX_LEN];

    const char *p;
    int size;
    char *separator;


    //
    // sanity checks
    //

    if (resource_name == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name(): NULL Resource Name passed in");
        return -1;
    }


    p = resource_name;

    // get the HAB-Type
    separator = strchr(p, '.');
    if (separator == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name(): error parsing Resource Name '%s'", resource_name);
        return -1;
    }
    size = separator - p;
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name(): HAB-Type of Resource Name '%s' has zero length", resource_name);
        return -1;
    }
    if (size > (sizeof(internal_hab_type) - 1)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name(): HAB-Type of Resource Name '%s' is too long (%d bytes, max %lu)", resource_name, size, (long unsigned)(sizeof(internal_hab_type) - 1));
        return -1;
    }
    memcpy(internal_hab_type, p, size);
    internal_hab_type[size] = '\0';

    if (! bionet_is_valid_name_component_or_wildcard(internal_hab_type)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name(): HAB-Type of Resource Name '%s' is not a valid name component or a wildcard", resource_name);
        return -1;
    }


    // get the HAB-ID
    p = separator + 1;
    separator = strchr(p, '.');
    if (separator == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name(): error parsing Resource Name '%s'", resource_name);
        return -1;
    }
    size = separator - p;
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name(): HAB-ID of Resource Name '%s' has zero length", resource_name);
        return -1;
    }
    if (size > (sizeof(internal_hab_id) - 1)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name(): HAB-ID of Resource Name '%s' is too long (%d bytes, max %lu)", resource_name, size, (long unsigned)(sizeof(internal_hab_id) - 1));
        return -1;
    }
    memcpy(internal_hab_id, p, size);
    internal_hab_id[size] = '\0';

    if (! bionet_is_valid_name_component_or_wildcard(internal_hab_id)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name(): HAB-ID of Resource Name '%s' is not a valid name component or a wildcard", resource_name);
        return -1;
    }


    // get the Node-ID
    p = separator + 1;
    separator = strchr(p, ':');
    if (separator == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name(): error parsing Resource Name'%s'", resource_name);
        return -1;
    }
    size = separator - p;
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name(): Node-ID of Resource Name '%s' has zero length", resource_name);
        return -1;
    }
    if (size > (sizeof(internal_node_id) - 1)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name(): Node-ID of Resource Name '%s' is too long (%d bytes, max %lu)", resource_name, size, (long unsigned)(sizeof(internal_node_id) - 1));
        return -1;
    }
    memcpy(internal_node_id, p, size);
    internal_node_id[size] = '\0';

    if (! bionet_is_valid_name_component_or_wildcard(internal_node_id)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name(): Node-ID of Resource Name '%s' is not a valid name component or a wildcard", resource_name);
        return -1;
    }


    // get the Resource-ID
    p = separator + 1;
    size = strlen(p);
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name(): Resource-ID of Resource Name '%s' has zero length", resource_name);
        return -1;
    }
    if (size > (sizeof(internal_resource_id) - 1)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name(): Resource-ID of Resource Name '%s' is too long (%d bytes, max %lu)", resource_name, size, (long unsigned)(sizeof(internal_resource_id) - 1));
        return -1;
    }
    memcpy(internal_resource_id, p, size);
    internal_resource_id[size] = '\0';

    if (! bionet_is_valid_name_component_or_wildcard(internal_resource_id)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name(): Resource-ID of Resource Name '%s' is not a valid name component or a wildcard", resource_name);
        return -1;
    }


    // set returned values, if the user wants them
    if (hab_type != NULL)    *hab_type = internal_hab_type;
    if (hab_id != NULL)      *hab_id = internal_hab_id;
    if (node_id != NULL)     *node_id = internal_node_id;
    if (resource_id != NULL) *resource_id = internal_resource_id;


    return 0;
}

int bionet_split_resource_name_r(
        const char * topic,
        char hab_type[BIONET_NAME_COMPONENT_MAX_LEN],
        char hab_id[BIONET_NAME_COMPONENT_MAX_LEN],
        char node_id[BIONET_NAME_COMPONENT_MAX_LEN],
        char resource_id[BIONET_NAME_COMPONENT_MAX_LEN])
{
    const char *p;
    int size;
    char *separator;


    //
    // sanity checks
    //
    if (NULL == topic) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name_r(): NULL Name Pattern passed in.");
	return -1;
    }


    p = topic;

    // get the HAB-Type
    separator = strchr(p, '.');
    if (separator == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name_r(): error parsing Topic '%s'", topic);
        return -1;
    }
    size = separator - p;
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name_r(): HAB-Type of Topic '%s' has zero length", topic);
        return -1;
    }
    if (size >= BIONET_NAME_COMPONENT_MAX_LEN) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name_r(): HAB-Type of Topic '%s' is too long (%d bytes, max %lu)", topic, size, (long unsigned)BIONET_NAME_COMPONENT_MAX_LEN);
        return -1;
    }
    memcpy(hab_type, p, size);
    hab_type[size] = '\0';

    if (! bionet_is_valid_name_component_or_wildcard(hab_type)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name_r(): HAB-Type of Topic '%s' is not a valid name component or a wildcard", topic);
        return -1;
    }


    // get the HAB-ID
    p = separator + 1;
    separator = strchr(p, '.');
    if (separator == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name_r(): error parsing Topic '%s'", topic);
        return -1;
    }
    size = separator - p;
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name_r(): HAB-ID of Topic '%s' has zero length", topic);
        return -1;
    }
    if (size >= BIONET_NAME_COMPONENT_MAX_LEN) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name_r(): HAB-ID of Topic '%s' is too long (%d bytes, max %lu)", topic, size, (long unsigned)BIONET_NAME_COMPONENT_MAX_LEN);
        return -1;
    }
    memcpy(hab_id, p, size);
    hab_id[size] = '\0';

    if (! bionet_is_valid_name_component_or_wildcard(hab_id)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name_r(): HAB-ID of Topic '%s' is not a valid name component or a wildcard", topic);
        return -1;
    }


    // get the Node-ID
    p = separator + 1;
    separator = strchr(p, ':');
    if (separator == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name_r(): error parsing Topic'%s'", topic);
        return -1;
    }
    size = separator - p;
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name_r(): Node-ID of Topic '%s' has zero length", topic);
        return -1;
    }
    if (size >= BIONET_NAME_COMPONENT_MAX_LEN) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name_r(): Node-ID of Topic '%s' is too long (%d bytes, max %lu)", topic, size, (long unsigned)BIONET_NAME_COMPONENT_MAX_LEN);
        return -1;
    }
    memcpy(node_id, p, size);
    node_id[size] = '\0';

    if (! bionet_is_valid_name_component_or_wildcard(node_id)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name_r(): Node-ID of Topic '%s' is not a valid name component or a wildcard", topic);
        return -1;
    }


    // get the Resource-ID
    int query_offset = 0;
    p = separator + 1;
    separator = strchr(p, '?');
    if (separator) {
        size = separator - p;
        query_offset = p-topic;
    } else {
        size = strlen(p);
    }
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name_r(): Resource-ID of Topic '%s' has zero length", topic);
        return -1;
    }
    if (size >= BIONET_NAME_COMPONENT_MAX_LEN) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name_r(): Resource-ID of Topic '%s' is too long (%d bytes, max %lu)", topic, size, (long unsigned)BIONET_NAME_COMPONENT_MAX_LEN);
        return -1;
    }
    memcpy(resource_id, p, size);
    resource_id[size] = '\0';

    if (! bionet_is_valid_name_component_or_wildcard(resource_id)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name_r(): Resource-ID of Topic '%s' is not a valid name component or a wildcard", topic);
        return -1;
    }

    return query_offset;
}

int bionet_split_nodeid_resourceid_r(
    const char *node_and_resource,
    char node_id[BIONET_NAME_COMPONENT_MAX_LEN],
    char resource_id[BIONET_NAME_COMPONENT_MAX_LEN]
) {
    const char *p;
    int size;
    char *separator;


    //
    // sanity checks
    //

    if (node_and_resource == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_nodeid_resourceid_r(): NULL input string passed in");
        return -1;
    }


    p = node_and_resource;


    // get the Node-ID
    separator = strchr(p, ':');
    if (separator == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_nodeid_resourceid_r(): error parsing input string '%s'", node_and_resource);
        return -1;
    }
    size = separator - p;
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_nodeid_resourceid_r(): Node-ID of input-string '%s' has zero length", node_and_resource);
        return -1;
    }
    if (size >= BIONET_NAME_COMPONENT_MAX_LEN) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_nodeid_resourceid_r(): Node-ID of input-string '%s' is too long (%d bytes, max %d)", node_and_resource, size, (BIONET_NAME_COMPONENT_MAX_LEN - 1));
        return -1;
    }
    memcpy(node_id, p, size);
    node_id[size] = '\0';


    // get the Resource-ID
    p = separator + 1;
    size = strlen(p);
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_nodeid_resourceid_r(): Resource-ID of input-string '%s' has zero length", node_and_resource);
        return -1;
    }
    if (size >= BIONET_NAME_COMPONENT_MAX_LEN) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_nodeid_resourceid_r(): Resource-ID of input-string '%s' is too long (%d bytes, max %d)", node_and_resource, size, (BIONET_NAME_COMPONENT_MAX_LEN - 1));
        return -1;
    }
    memcpy(resource_id, p, size);
    resource_id[size] = '\0';


    return 0;
}




int bionet_split_node_name(
    const char *node_name,
    char **hab_type,
    char **hab_id,
    char **node_id
) {
    static char internal_hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
    static char internal_hab_id[BIONET_NAME_COMPONENT_MAX_LEN];
    static char internal_node_id[BIONET_NAME_COMPONENT_MAX_LEN];

    const char *p;
    int size;
    char *separator;


    //
    // sanity checks
    //

    if (node_name == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): NULL Node Name passed in");
        return -1;
    }


    p = node_name;

    // get the HAB-Type
    separator = strchr(p, '.');
    if (separator == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): error parsing Node Name '%s'", node_name);
        return -1;
    }
    size = separator - p;
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): HAB-Type of Node Name '%s' has zero length", node_name);
        return -1;
    }
    if (size >= BIONET_NAME_COMPONENT_MAX_LEN) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): HAB-Type of Node Name '%s' is too long (%d bytes, max %lu)", node_name, size, (long unsigned)BIONET_NAME_COMPONENT_MAX_LEN);
        return -1;
    }
    memcpy(internal_hab_type, p, size);
    internal_hab_type[size] = '\0';

    if (! bionet_is_valid_name_component_or_wildcard(internal_hab_type)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): HAB-Type of Node Name '%s' is not a valid name component or a wildcard", node_name);
        return -1;
    }


    // get the HAB-ID
    p = separator + 1;
    separator = strchr(p, '.');
    if (separator == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): error parsing Node Name '%s'", node_name);
        return -1;
    }
    size = separator - p;
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): HAB-ID of Node Name '%s' has zero length", node_name);
        return -1;
    }
    if (size >= BIONET_NAME_COMPONENT_MAX_LEN) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): HAB-ID of Node Name '%s' is too long (%d bytes, max %lu)", node_name, size, (long unsigned)BIONET_NAME_COMPONENT_MAX_LEN);
        return -1;
    }
    memcpy(internal_hab_id, p, size);
    internal_hab_id[size] = '\0';

    if (! bionet_is_valid_name_component_or_wildcard(internal_hab_id)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): HAB-ID of Node Name '%s' is not a valid name component or a wildcard", node_name);
        return -1;
    }


    // get the Node-ID
    p = separator + 1;
    size = strlen(p);
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): Node-ID of Node Name '%s' has zero length", node_name);
        return -1;
    }
    if (size >= BIONET_NAME_COMPONENT_MAX_LEN) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): Node-ID of Node Name '%s' is too long (%d bytes, max %lu)", node_name, size, (long unsigned)BIONET_NAME_COMPONENT_MAX_LEN);
        return -1;
    }
    memcpy(internal_node_id, p, size);
    internal_node_id[size] = '\0';

    if (! bionet_is_valid_name_component_or_wildcard(internal_node_id)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): Node-ID of Node Name '%s' is not a valid name component or a wildcard", node_name);
        return -1;
    }


    // set returned values
    if (hab_type != NULL) *hab_type = internal_hab_type;
    if (hab_id != NULL)   *hab_id = internal_hab_id;
    if (node_id != NULL)  *node_id = internal_node_id;

    return 0;
}

int bionet_split_node_name_r(
    const char *node_name,
    char hab_type[BIONET_NAME_COMPONENT_MAX_LEN],
    char hab_id[BIONET_NAME_COMPONENT_MAX_LEN],
    char node_id[BIONET_NAME_COMPONENT_MAX_LEN])
{
    const char *p;
    int size;
    char *separator;


    //
    // sanity checks
    //

    if (node_name == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): NULL Node Name passed in");
        return -1;
    }


    p = node_name;

    // get the HAB-Type
    separator = strchr(p, '.');
    if (separator == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): error parsing Node Name '%s'", node_name);
        return -1;
    }
    size = separator - p;
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): HAB-Type of Node Name '%s' has zero length", node_name);
        return -1;
    }
    if (size >= BIONET_NAME_COMPONENT_MAX_LEN) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): HAB-Type of Node Name '%s' is too long (%d bytes, max %lu)", node_name, size, (long unsigned)BIONET_NAME_COMPONENT_MAX_LEN);
        return -1;
    }
    if ( hab_type) {
        memcpy(hab_type, p, size);
        hab_type[size] = '\0';

        if (! bionet_is_valid_name_component_or_wildcard(hab_type)) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): HAB-Type of Node Name '%s' is not a valid name component or a wildcard", node_name);
            return -1;
        }
    }


    // get the HAB-ID
    p = separator + 1;
    separator = strchr(p, '.');
    if (separator == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): error parsing Node Name '%s'", node_name);
        return -1;
    }
    size = separator - p;
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): HAB-ID of Node Name '%s' has zero length", node_name);
        return -1;
    }
    if (size >= BIONET_NAME_COMPONENT_MAX_LEN) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): HAB-ID of Node Name '%s' is too long (%d bytes, max %lu)", node_name, size, (long unsigned)BIONET_NAME_COMPONENT_MAX_LEN);
        return -1;
    }
    if (hab_id){
        memcpy(hab_id, p, size);
        hab_id[size] = '\0';

        if (! bionet_is_valid_name_component_or_wildcard(hab_id)) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): HAB-ID of Node Name '%s' is not a valid name component or a wildcard", node_name);
            return -1;
        }
    }


    // get the Node-ID
    p = separator + 1;
    size = strlen(p);
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): Node-ID of Node Name '%s' has zero length", node_name);
        return -1;
    }
    if (size >= BIONET_NAME_COMPONENT_MAX_LEN) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): Node-ID of Node Name '%s' is too long (%d bytes, max %lu)", node_name, size, (long unsigned)BIONET_NAME_COMPONENT_MAX_LEN);
        return -1;
    }
    if(node_id){
        memcpy(node_id, p, size);
        node_id[size] = '\0';

        if (! bionet_is_valid_name_component_or_wildcard(node_id)) {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): Node-ID of Node Name '%s' is not a valid name component or a wildcard", node_name);
            return -1;
        }
    }

    return 0;
}




int bionet_split_hab_name(
    const char *hab_name,
    char **hab_type,
    char **hab_id
) {
    static char internal_hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
    static char internal_hab_id[BIONET_NAME_COMPONENT_MAX_LEN];

    const char *p;
    int size;
    char *separator;


    //
    // sanity checks
    //

    if (hab_name == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_hab_name(): NULL HAB Name passed in");
        return -1;
    }


    p = hab_name;

    // get the HAB-Type
    separator = strchr(p, '.');
    if (separator == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_hab_name(): error parsing HAB Name '%s'", hab_name);
        return -1;
    }
    size = separator - p;
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_hab_name(): HAB-Type of HAB Name '%s' has zero length", hab_name);
        return -1;
    }
    if (size > (sizeof(internal_hab_type) - 1)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_hab_name(): HAB-Type of HAB Name '%s' is too long (%d, max %lu)", hab_name, size, (long unsigned)(sizeof(internal_hab_type) - 1));
        return -1;
    }
    memcpy(internal_hab_type, p, size);
    internal_hab_type[size] = '\0';

    if (! bionet_is_valid_name_component_or_wildcard(internal_hab_type)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_hab_name(): HAB-Type of HAB Name '%s' is not a valid name component or a wildcard", hab_name);
        return -1;
    }


    // get the HAB-ID
    p = separator + 1;
    size = strlen(p);
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_hab_name(): HAB-ID of HAB Name '%s' has zero length", hab_name);
        return -1;
    }
    if (size > (sizeof(internal_hab_id) - 1)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_hab_name(): HAB-ID of HAB Name pattern '%s' is too long (%d, max %lu)", hab_name, size, (long unsigned)(sizeof(internal_hab_id) - 1));
        return -1;
    }
    memcpy(internal_hab_id, p, size);
    internal_hab_id[size] = '\0';

    if (! bionet_is_valid_name_component_or_wildcard(internal_hab_id)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_hab_name(): HAB-ID of HAB Name '%s' is not a valid name component or a wildcard", hab_name);
        return -1;
    }


    // set returned values
    if (hab_type != NULL) *hab_type = internal_hab_type;
    if (hab_id != NULL)   *hab_id = internal_hab_id;


    return 0;
}


int bionet_split_hab_name_r(
    const char *hab_name,
    char hab_type[BIONET_NAME_COMPONENT_MAX_LEN],
    char hab_id[BIONET_NAME_COMPONENT_MAX_LEN]
) {
    const char *p;
    int size;
    char *separator;

    int max_size = (BIONET_NAME_COMPONENT_MAX_LEN - 1);


    //
    // sanity checks
    //

    if (hab_name == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_hab_name_r(): NULL HAB Name passed in");
        return -1;
    }

    if (hab_type == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_hab_name_r(): NULL hab_type passed in");
        return -1;
    }

    if (hab_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_hab_name_r(): NULL hab_id passed in");
        return -1;
    }


    p = hab_name;

    // get the HAB-Type
    separator = strchr(p, '.');
    if (separator == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_hab_name_r(): error parsing HAB Name '%s'", hab_name);
        return -1;
    }
    size = separator - p;
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_hab_name_r(): HAB-Type of HAB Name '%s' has zero length", hab_name);
        return -1;
    }
    if (size > max_size) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_hab_name_r(): HAB-Type of HAB Name '%s' is too long (%d, max %d)", hab_name, size, max_size);
        return -1;
    }
    memcpy(hab_type, p, size);
    hab_type[size] = '\0';


    // get the HAB-ID
    p = separator + 1;
    size = strlen(p);
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_hab_name_r(): HAB-ID of HAB Name '%s' has zero length", hab_name);
        return -1;
    }
    if (size > max_size) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_hab_name_r(): HAB-ID of HAB Name pattern '%s' is too long (%d, max %d)", hab_name, size, max_size);
        return -1;
    }
    memcpy(hab_id, p, size);
    hab_id[size] = '\0';


    return 0;
}


int bionet_split_name_components_r(const char * name_pattern,
				   char hab_type[BIONET_NAME_COMPONENT_MAX_LEN],
				   char hab_id[BIONET_NAME_COMPONENT_MAX_LEN],
				   char node_id[BIONET_NAME_COMPONENT_MAX_LEN],
				   char resource_id[BIONET_NAME_COMPONENT_MAX_LEN]) {
    const char *p;
    int size;
    char *separator;

    hab_type[0] = '\0';
    hab_id[0] = '\0';
    node_id[0] = '\0';
    resource_id[0] = '\0';

    //
    // sanity checks
    //

    if (NULL == name_pattern) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_name_components_r(): NULL Name Pattern passed in.");
	return -1;
    }

    p = name_pattern;

    // get the HAB-Type
    separator = strchr(p, '.');
    if (separator == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_name_components_r(): error parsing Name Pattern '%s'", name_pattern);
        return -1;
    }
    size = separator - p;
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_name_components_r(): HAB-Type of Name Pattern '%s' has zero length", name_pattern);
        return -1;
    }
    if (size >= BIONET_NAME_COMPONENT_MAX_LEN) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_name_components_r(): HAB-Type of Name Pattern '%s' is too long (%d bytes, max %lu)", name_pattern, size, (long unsigned)BIONET_NAME_COMPONENT_MAX_LEN);
        return -1;
    }
    memcpy(hab_type, p, size);
    hab_type[size] = '\0';

    if (! bionet_is_valid_name_component_or_wildcard(hab_type)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_name_components_r(): HAB-Type of Name Pattern '%s' is not a valid name component or a wildcard", name_pattern);
        return -1;
    }


    // get the HAB-ID
    p = separator + 1;
    separator = strchr(p, '.');
    if (separator == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "bionet_split_name_components_r(): error parsing Name Pattern '%s'", name_pattern);
	size = (name_pattern) + strlen(name_pattern) - p;
	if (size >= BIONET_NAME_COMPONENT_MAX_LEN) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_name_components_r(): HAB-ID of Name Pattern '%s' is too long (%d bytes, max %lu)", name_pattern, size, (long unsigned)BIONET_NAME_COMPONENT_MAX_LEN);
	    return -1;
	}
	memcpy(hab_id, p, size);
	hab_id[size] = '\0';
	
	if (! bionet_is_valid_name_component_or_wildcard(hab_id)) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_name_components_r(): HAB-ID of Name Pattern '%s' is not a valid name component or a wildcard", name_pattern);
	    return -1;
	}
	return 0;
    }
    size = separator - p;
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_name_components_r(): HAB-ID of Name Pattern '%s' has zero length", name_pattern);
        return -1;
    }
    if (size >= BIONET_NAME_COMPONENT_MAX_LEN) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_name_components_r(): HAB-ID of Name Pattern '%s' is too long (%d bytes, max %lu)", name_pattern, size, (long unsigned)BIONET_NAME_COMPONENT_MAX_LEN);
        return -1;
    }
    memcpy(hab_id, p, size);
    hab_id[size] = '\0';

    if (! bionet_is_valid_name_component_or_wildcard(hab_id)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_name_components_r(): HAB-ID of Name Pattern '%s' is not a valid name component or a wildcard", name_pattern);
        return -1;
    }


    // get the Node-ID
    p = separator + 1;
    separator = strchr(p, ':');
    if (separator == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "bionet_split_name_components_r(): error parsing Name Pattern'%s'", name_pattern);
	size = (name_pattern) + strlen(name_pattern) - p;
	if (size >= BIONET_NAME_COMPONENT_MAX_LEN) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_name_components_r(): Node-ID of Name Pattern '%s' is too long (%d bytes, max %lu)", name_pattern, size, (long unsigned)BIONET_NAME_COMPONENT_MAX_LEN);
	    return -1;
	}
	memcpy(node_id, p, size);
	node_id[size] = '\0';
	
	if (! bionet_is_valid_name_component_or_wildcard(node_id)) {
	    g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_name_components_r(): Node-ID of Name Pattern '%s' is not a valid name component or a wildcard", name_pattern);
	    return -1;
	}
	return 0;
    }
    size = separator - p;
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_name_components_r(): Node-ID of Name Pattern '%s' has zero length", name_pattern);
        return -1;
    }
    if (size >= BIONET_NAME_COMPONENT_MAX_LEN) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_name_components_r(): Node-ID of Name Pattern '%s' is too long (%d bytes, max %lu)", name_pattern, size, (long unsigned)BIONET_NAME_COMPONENT_MAX_LEN);
        return -1;
    }
    memcpy(node_id, p, size);
    node_id[size] = '\0';

    if (! bionet_is_valid_name_component_or_wildcard(node_id)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_name_components_r(): Node-ID of Name Pattern '%s' is not a valid name component or a wildcard", name_pattern);
        return -1;
    }


    // get the Resource-ID
    p = separator + 1;
    separator = strchr(p, '?');
    if (separator) {
        size = separator - p;
    } else {
        size = strlen(p);
    }
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "bionet_split_name_components_r(): Resource-ID of Name Pattern '%s' has zero length", name_pattern);
        return 0;
    }
    if (size >= BIONET_NAME_COMPONENT_MAX_LEN) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_name_components_r(): Resource-ID of Name Pattern '%s' is too long (%d bytes, max %lu)", name_pattern, size, (long unsigned)BIONET_NAME_COMPONENT_MAX_LEN);
        return -1;
    }
    memcpy(resource_id, p, size);
    resource_id[size] = '\0';

    if (! bionet_is_valid_name_component_or_wildcard(resource_id)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_name_components_r(): Resource-ID of Name Pattern '%s' is not a valid name component or a wildcard", name_pattern);
        return -1;
    }

    return 0;
} /* bionet_split_name_components_r() */


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
