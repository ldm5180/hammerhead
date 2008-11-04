
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#ifndef BIONET_UTIL_H
#define BIONET_UTIL_H


#include <stdint.h>
#include <time.h>

#include <sys/time.h>

#include <glib.h>




typedef struct bionet_hab       bionet_hab_t;
typedef struct bionet_node      bionet_node_t;
typedef struct bionet_stream    bionet_stream_t;
typedef struct bionet_resource  bionet_resource_t;
typedef struct bionet_datapoint bionet_datapoint_t;


#include "bionet-hab.h"
#include "bionet-node.h"
#include "bionet-stream.h"
#include "bionet-resource.h"
#include "bionet-datapoint.h"




//
// The bionet code uses glib's logging facility to log internal messages,
// and this is the domain.
//

#define  BIONET_LOG_DOMAIN  "bionet"




//
// bionet server listens on this TCP port, clients connect
//

#define BIONET_PORT (11002)




//
// name components are no more than this long (including the terminating NULL)
//

#define BIONET_NAME_COMPONENT_MAX_LEN (100)




// 
// OS convergence layer for error reporting
//

int bionet_get_network_error(void);
const char *bionet_get_network_error_string(void);




// 
// misc helpers
//

int bionet_is_valid_name_component_or_wildcard(const char *str);
int bionet_is_valid_name_component(const char *str);

int bionet_name_component_matches(const char *name_component, const char *pattern);




//
// puts the glib log messages where you want them
//

typedef struct {
    enum {
        BIONET_LOG_TO_STDOUT = 0,
        BIONET_LOG_TO_SYSLOG = 1
    } destination;

    // messages with log_level *below* log_limit are logged, all others are dropped
    // FIXME: really we want a default log_limit and then optional per-domain limits
    GLogLevelFlags log_limit;
} bionet_log_context_t;

void bionet_glib_log_handler(
    const gchar *log_domain,
    GLogLevelFlags log_level,
    const gchar *message,
    gpointer log_context
);




// 
// These functions take a string containing a name, split the name up into
// its components, and return the components.
//
// They return 0 on success, -1 on error.
//

int bionet_split_resource_name(
    const char *resource_name,
    char **hab_type,
    char **hab_id,
    char **node_id,
    char **resource_id
);

int bionet_split_node_name(
    const char *node_name,
    char **hab_type,
    char **hab_id,
    char **node_id
);

int bionet_split_hab_name(
    const char *hab_name,
    char **hab_type,
    char **hab_id
);

int bionet_split_hab_name_r(
    const char *hab_name,
    char hab_type[BIONET_NAME_COMPONENT_MAX_LEN],
    char hab_id[BIONET_NAME_COMPONENT_MAX_LEN]
);




#endif //  BIONET_UTIL_H

