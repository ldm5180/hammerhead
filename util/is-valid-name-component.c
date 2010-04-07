
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>

#include "internal.h"
#include "bionet-util.h"



int bionet_is_valid_name_component(const char *str) {
    if (str == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_is_valid_name_component(): NULL string passed in!");
        return 0;
    }

    // zero-length string is NOT valid
    if (*str == '\0') return 0;

    // too long?
    if (strlen(str) > BIONET_NAME_COMPONENT_MAX_LEN) return 0;

    while (*str != '\0') {
        if (!isalnum(*str) && (*str != '-')) return 0;
        str ++;
    }

    return 1;
}


int bionet_is_valid_name_component_or_wildcard(const char *str) {
    if (str == NULL) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_is_valid_name_component_or_wildcard(): NULL string passed in!");
        return 0;
    }

    // zero-length string is NOT valid
    if (*str == '\0') return 0;

    // too long?
    if (strlen(str) > BIONET_NAME_COMPONENT_MAX_LEN) return 0;

    if (strcmp(str, "*") == 0) {
        return 1;
    }

    while (*str != '\0') {
        if (!isalnum(*str) && (*str != '-')) return 0;
        str ++;
    }

    return 1;
}


int bionet_name_component_matches(const char *name_component, const char *pattern) {
    if (!bionet_is_valid_name_component_or_wildcard(name_component)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_name_component_matches(): invalid name-component '%s' passed in!", name_component);
        return 0;
    }

    if (!bionet_is_valid_name_component_or_wildcard(pattern)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_name_component_matches(): invalid name-component-pattern '%s' passed in!", pattern);
        return 0;
    }

    if ((strcmp(pattern, "*") == 0) || strcmp(name_component, "*") == 0) return 1;
    if (strcmp(pattern, name_component) == 0) return 1;

    return 0;
}

int bionet_resource_name_matches(const char *resource_name, const char *pattern) {
    char hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
    char hab_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char node_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char resource_id[BIONET_NAME_COMPONENT_MAX_LEN];

    char pattern_hab_type[BIONET_NAME_COMPONENT_MAX_LEN];
    char pattern_hab_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char pattern_node_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char pattern_resource_id[BIONET_NAME_COMPONENT_MAX_LEN];

    if (bionet_split_resource_name_r(resource_name,
				     hab_type, hab_id, node_id, resource_id)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_name_matches(): Failed to split resource name.");
	return 0;
    }

    if (bionet_split_resource_name_r(pattern,
				   pattern_hab_type, pattern_hab_id, pattern_node_id, pattern_resource_id)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_resource_name_matches(): Failed to split resource name pattern.");
	return 0;
    }

    if (0 == bionet_name_component_matches(hab_type, pattern_hab_type)) {
	return 0;
    }

    if (0 == bionet_name_component_matches(hab_id, pattern_hab_id)) {
	return 0;
    }

    if (0 == bionet_name_component_matches(node_id, pattern_node_id)) {
	return 0;
    }

    if (0 == bionet_name_component_matches(resource_id, pattern_resource_id)) {
	return 0;
    }

    //match!
    return 1;
}
