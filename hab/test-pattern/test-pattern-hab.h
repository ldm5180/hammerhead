
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef __BDM_H
#define __BDM_H


#include <stdint.h>
#include <unistd.h>

#include <glib.h>

#include "hardware-abstractor.h"
#include "bionet.h"

#include "parser.h"

typedef enum {
    NEW_NODE,
    DATAPOINT_UPDATE,
    LOST_NODE
} event_type;

typedef enum {
    BINARY,
    UINT8,
    INT8,
    UINT16,
    INT16,
    UINT32,
    INT32,
    FLOAT,
    DOUBLE,
    STRING
} value_type;

struct event_t {
    event_type type;
    struct timeval *tv;
    void *event;
};

struct new_node_event_t {
    char id[BIONET_NAME_COMPONENT_MAX_LEN];
    GSList *resources;
};

struct lost_node_event_t {
    char id[BIONET_NAME_COMPONENT_MAX_LEN];
};

struct resource_info_t {
    char id[BIONET_NAME_COMPONENT_MAX_LEN];
    bionet_resource_flavor_t flavor;
    bionet_resource_data_type_t data_type;
    gboolean has_value;
    void *value;
};

struct datapoint_event_t {
    char node_id[BIONET_NAME_COMPONENT_MAX_LEN];
    char id[BIONET_NAME_COMPONENT_MAX_LEN];
    char *value;
};

/* globals...booo */
bionet_hab_t *hab;
GSList *events;

gint timeval_diff(gconstpointer a, gconstpointer b);
gboolean dump_tree(gpointer key, gpointer value, gpointer data);
void simulate_updates(gpointer data, gpointer user_data);
bionet_value_t *str_to_value(bionet_resource_t *resource,
    bionet_resource_data_type_t data_type,
    char *str);
    

void str_to_timeval(const char *str, struct timeval *tv);

struct event_t* read_event(int fd);

struct resource_info_t *create_resource(char *id, 
    bionet_resource_data_type_t data_type, 
    bionet_resource_flavor_t flavor, 
    char *value);
struct resource_info_t *create_empty_resource(char *id, 
    bionet_resource_data_type_t data_type, 
    bionet_resource_flavor_t flavor);

struct event_t *add_node_event(struct timeval *tv, char *id, GSList *resources);
struct event_t *remove_node_event(struct timeval *tv, char *id);
struct event_t *update_event(struct timeval *tv, char *node, char *resource, char *value);


extern int yyparse();
extern FILE* yyin;
extern void yyrestart(FILE *input_file);

#endif
