
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


// 
// The functions in this file take strings containing the names of various
// Bionet objects and split the strings into the components of the names.
//


#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "bionet-util.h"




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

    if (hab_type == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name(): NULL hab_type passed in");
        return -1;
    }

    if (hab_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name(): NULL hab_id passed in");
        return -1;
    }

    if (node_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name(): NULL node_id passed in");
        return -1;
    }

    if (resource_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_resource_name(): NULL resource_id passed in");
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


    // set returned values
    *hab_type = internal_hab_type;
    *hab_id = internal_hab_id;
    *node_id = internal_node_id;
    *resource_id = internal_resource_id;


    return 0;
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
    if (size > (BIONET_NAME_COMPONENT_MAX_LEN - 1)) {
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
    if (size > (BIONET_NAME_COMPONENT_MAX_LEN - 1)) {
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

    if (hab_type == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): NULL hab_type passed in");
        return -1;
    }

    if (hab_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): NULL hab_id passed in");
        return -1;
    }

    if (node_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): NULL node_id passed in");
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
    if (size > (sizeof(internal_hab_type) - 1)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): HAB-Type of Node Name '%s' is too long (%d bytes, max %lu)", node_name, size, (long unsigned)(sizeof(internal_hab_type) - 1));
        return -1;
    }
    memcpy(internal_hab_type, p, size);
    internal_hab_type[size] = '\0';


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
    if (size > (sizeof(internal_hab_id) - 1)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): HAB-ID of Node Name '%s' is too long (%d bytes, max %lu)", node_name, size, (long unsigned)(sizeof(internal_hab_id) - 1));
        return -1;
    }
    memcpy(internal_hab_id, p, size);
    internal_hab_id[size] = '\0';


    // get the Node-ID
    p = separator + 1;
    size = strlen(p);
    if (size == 0) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): Node-ID of Node Name '%s' has zero length", node_name);
        return -1;
    }
    if (size > (sizeof(internal_node_id) - 1)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_node_name(): Node-ID of Node Name '%s' is too long (%d bytes, max %lu)", node_name, size, (long unsigned)(sizeof(internal_node_id) - 1));
        return -1;
    }
    memcpy(internal_node_id, p, size);
    internal_node_id[size] = '\0';


    // set returned values
    *hab_type = internal_hab_type;
    *hab_id = internal_hab_id;
    *node_id = internal_node_id;


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

    if (hab_type == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_hab_name(): NULL hab_type passed in");
        return -1;
    }

    if (hab_id == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_split_hab_name(): NULL hab_id passed in");
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


    // set returned values
    *hab_type = internal_hab_type;
    *hab_id = internal_hab_id;


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

