
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef BIONET_DATA_MANAGER_H
#define BIONET_DATA_MANAGER_H


#include <glib.h>

#include "bionet.h"

#include "bionet-asn.h"
#include "bionet-util.h"


// Number of bytes to use for the resource key
// from the sha1 hash. Must be <= to SHA_DIGEST_LENGTH;
#define BDM_RESOURCE_KEY_LENGTH 8 


extern char *bdm_pidfile;
extern GMainLoop *bdm_main_loop;


typedef struct {
    struct timeval entry_ts;
    bionet_datapoint_t * datapoint;
} bdm_record_t;

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
// Insert a datapoint, possibly before the metadata is available
//
int db_add_datapoint_sync(
    uint8_t resource_key[BDM_RESOURCE_KEY_LENGTH],
    const char * bdm_id,
    const char * value, 
    struct timeval *timestamp);



// 
// Finds all matching datapoints, and returns them as a GPtrArray of HABs.
//

GPtrArray *db_get_resource_datapoints(
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const char *resource_id,
    struct timeval *datapoint_start,
    struct timeval *datapoint_end,
    struct timeval *entry_start,
    struct timeval *entry_end
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
    union {
	BDM_C2S_Message_t             *C2S_message;
	BDM_Sync_Datapoints_Message_t *sync_dp_message;
	BDM_Sync_Metadata_Message_t   *sync_metadata_message;
    } message;
    unsigned char buffer[(10 * 1024)];  // FIXME: use cal
    int index;
} client_t;


int client_connecting_handler(GIOChannel *ch, GIOCondition condition, gpointer listening_fd_as_pointer);
int client_readable_handler(GIOChannel *unused, GIOCondition unused2, client_t *client);
void handle_client_message(client_t *client, BDM_C2S_Message_t *message);
void disconnect_client(client_t *client);

int sync_receive_connecting_handler(GIOChannel *ch, GIOCondition condition, gpointer listening_fd_as_pointer);
int sync_receive_readable_handler(GIOChannel *unused, GIOCondition unused2, client_t *client);
void handle_sync_datapoints_message(client_t *client, BDM_Sync_Datapoints_Message_t *message);
void handle_sync_metadata_message(client_t *client, BDM_Sync_Metadata_Message_t *message);



int make_listening_socket(int port);
void keepalive(int socket, int idle, int count, int interval);
void make_shutdowns_clean(void);


// BDM sync'ing
int sync_send_metadata();
int sync_send_datapoints();


#endif //  BIONET_DATA_MANAGER_H

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
