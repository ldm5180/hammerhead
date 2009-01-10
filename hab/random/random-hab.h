
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#ifndef RANDOM_HAB_H
#define RANDOM_HAB_H


#define HAB_TYPE "random-hab"


typedef enum {
    OM_NORMAL,
    OM_BDM_CLIENT,
    OM_BIONET_WATCHER
} om_t;

extern om_t output_mode;


void new_node_with_resources(bionet_hab_t* random_hab);
void destroy_node(bionet_hab_t* random_hab);


void usage(int exit_val);
const char *get_random_word(void);
bionet_node_t *pick_random_node(bionet_hab_t *random_hab);
void add_node(bionet_hab_t* random_hab);
void update_node(bionet_hab_t* random_hab);
void set_random_resource_value(bionet_resource_t *resource);


#endif //  RANDOM_HAB_H

