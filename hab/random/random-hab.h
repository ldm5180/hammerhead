
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef RANDOM_HAB_H
#define RANDOM_HAB_H


#define HAB_TYPE "random-hab"


typedef enum {
    OM_NORMAL,
    OM_BDM_CLIENT,
    OM_BIONET_WATCHER
} om_t;

extern om_t output_mode;
extern int sorted_resources;


void new_node_with_resources(bionet_hab_t* random_hab);
void destroy_node(bionet_hab_t* random_hab);


void usage();
const char *get_random_word(void);
bionet_node_t *pick_random_node(bionet_hab_t *random_hab);
void add_node(bionet_hab_t* random_hab);
void update_node(bionet_hab_t* random_hab);
void set_random_resource_value(bionet_resource_t *resource);

char * timeval_as_str(
    struct timeval *tv,
    char * time_str, size_t size);

#endif //  RANDOM_HAB_H

