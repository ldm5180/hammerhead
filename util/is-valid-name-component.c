
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>

#ifdef BIONET_21_API
#include "internal.h"
#include "bionet-util-2.1.h"
#else
#include "bionet-util.h"
#endif



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
    if (!bionet_is_valid_name_component(name_component)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_name_component_matches(): invalid name-component '%s' passed in!", name_component);
        return 0;
    }

    if (!bionet_is_valid_name_component_or_wildcard(pattern)) {
        g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "bionet_name_component_matches(): invalid name-component-pattern '%s' passed in!", pattern);
        return 0;
    }

    if (strcmp(pattern, "*") == 0) return 1;
    if (strcmp(pattern, name_component) == 0) return 1;

    return 0;
}

