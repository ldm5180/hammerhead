
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef BIONET_DATA_MANAGER_H
#define BIONET_DATA_MANAGER_H


#include <glib.h>

#include "bionet.h"

#include "bionet-asn.h"
#include "bionet-util.h"

#include "config.h"

#if ENABLE_ION
#include "ion/zco.h"
#include "ion/sdr.h"
#include "ion/bp.h"
#endif


// Number of bytes to use for the resource key
// from the sha1 hash. Must be <= to SHA_DIGEST_LENGTH;
#define BDM_RESOURCE_KEY_LENGTH 8 


extern char *bdm_pidfile;
extern GMainLoop *bdm_main_loop;

extern GHashTable * bdm_opts_table;


typedef struct {
    struct timeval entry_ts;
    bionet_datapoint_t * datapoint;
} bdm_record_t;

typedef enum {
    BDM_SYNC_METHOD_TCP = 0,
    BDM_SYNC_METHOD_ION = 1,
} bdm_sync_method_t;

typedef struct {
    bdm_sync_method_t method;  // The method used to send
    struct timeval start_time; // Start-time datapoint filter
    struct timeval end_time;   // End-time datapoint filter
    char resource_name_pattern[BIONET_NAME_COMPONENT_MAX_LEN * 4];
    unsigned int frequency;
    char * sync_recipient;
    int remote_port;
#if ENABLE_ION
    // Group ion configs for clarity
    struct {
        int basekey; // ION base key, to allow multiple instance on a machine
        Sdr	sdr; // The SDR of the open transaction. NULL when not in transaction
        Object	bundleZco; // The ZCO that is being appended to.
    } ion;
#endif

    //State vars
    int last_entry_end_seq;    
    int last_entry_end_seq_metadata; 
    int fd;
} sync_sender_config_t;

typedef struct { 
    GPtrArray * hab_list;
    char * bdm_id;
} bdm_t;

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
// Make a resource key
//
int db_make_resource_key(
    const char * hab_type,
    const char * hab_id,
    const char * node_id,
    const char * resource_id,
    bionet_resource_data_type_t data_type,
    bionet_resource_flavor_t flavor,
    uint8_t resource_key[BDM_RESOURCE_KEY_LENGTH]);

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
    struct timeval *timestamp,
    bionet_resource_data_type_t type,
    void * value);


// 
// Finds all matching datapoints, and returns them as a GPtrArray of bdm_t.
//
GPtrArray *db_get_resource_datapoints(
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const char *resource_id,
    struct timeval *datapoint_start,
    struct timeval *datapoint_end,
    int entry_start,
    int entry_end,
    int *latest_entry
);

// 
// Finds all matching metadata, and returns them as a GPtrArray of bdm_t.
//
GPtrArray *db_get_metadata(
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const char *resource_id,
    struct timeval *datapoint_start,
    struct timeval *datapoint_end,
    int entry_start,
    int entry_end,
    int *latest_entry);

int db_get_latest_entry_seq(void);
int db_get_last_sync_seq(char * bdm_id);
void db_set_last_sync_seq(char * bdm_id, int seq);

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
	BDM_Sync_Message_t            *sync_message;
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
gpointer sync_thread(gpointer config);

extern GSList * sync_config_list;

sync_sender_config_t * read_config_file(const char * fname);

#endif //  BIONET_DATA_MANAGER_H

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
