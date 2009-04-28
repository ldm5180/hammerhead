
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef BIONET_DATA_MANAGER_H
#define BIONET_DATA_MANAGER_H


#include <glib.h>

#include "bionet.h"

#include "bionet-asn.h"
#include "bionet-util.h"




extern char *bdm_pidfile;
extern GMainLoop *bdm_main_loop;




// 
// interface to the database backend
//


//
// set up the database
//
// return 0 on success, -1 on failure
//

int db_init(void);


// 
// shut down the database
//

void db_shutdown(void);


//
// Each of these functions inserts a bionet object and, if needed, all its
// parent objects.  These functions are idempotent.
//
// Return 0 on success, -1 on failure.
//

int db_add_datapoint(bionet_datapoint_t *datapoint);
int db_add_resource(bionet_resource_t *resource);
int db_add_node(bionet_node_t *node);
int db_add_hab(bionet_hab_t *hab);




// 
// Finds all matching datapoints, and returns them as a GPtrArray of HABs.
//

GPtrArray *db_get_resource_datapoints(
    const unsigned char *hab_type,
    const unsigned char *hab_id,
    const unsigned char *node_id,
    const unsigned char *resource_id,
    struct timeval *start,
    struct timeval *end
);




// 
// stuff for being a bionet client
//

#define MAX_SUBSCRIPTIONS 100
extern int hab_list_index;
extern char *hab_list_name_patterns[MAX_SUBSCRIPTIONS];

extern int node_list_index;
extern char *node_list_name_patterns[MAX_SUBSCRIPTIONS];

extern int resource_index;
extern char *resource_name_patterns[MAX_SUBSCRIPTIONS];

extern char * security_dir;
extern int require_security;

int try_to_connect_to_bionet(void *unused);




// 
// stuff to actually be BDM
//


typedef struct {
    int fd;
    GIOChannel *ch;

    // keep track of messages coming in from the client
    BDM_C2S_Message_t *message;
    unsigned char buffer[(10 * 1024)];  // FIXME: use cal
    int index;
} client_t;


int client_connecting_handler(GIOChannel *ch, GIOCondition condition, gpointer listening_fd_as_pointer);
int client_readable_handler(GIOChannel *unused, GIOCondition unused2, client_t *client);
void handle_client_message(client_t *client, BDM_C2S_Message_t *message);
void disconnect_client(client_t *client);


int make_listening_socket(int port);
void keepalive(int socket, int idle, int count, int interval);
void make_shutdowns_clean(void);


#endif //  BIONET_DATA_MANAGER_H

