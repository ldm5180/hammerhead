
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef BIONET_DATA_MANAGER_H
#define BIONET_DATA_MANAGER_H

#include "config.h"

#include <glib.h>

#include "bionet.h"

#include "bionet-asn.h"
#include "bionet-util.h"
#include "cal-event.h"


// Default bundle lifetime in seconds.
#define BDM_BUNDLE_LIFETIME (300)

#if ENABLE_ION
#include "zco.h"
#include "sdr.h"
#include "bp.h"

typedef void (*sm_set_basekey_t)(unsigned int key);
typedef int (*bp_attach_t)(void);
typedef void (*bp_interrupt_t)(BpSAP sap);
typedef int (*bp_open_t)(char *eid, BpSAP *ionsapPtr);
typedef void (*bp_close_t)(BpSAP sap);
typedef Sdr (*bp_get_sdr_t)(void);
typedef int (*bp_send_t)(BpSAP sap, int mode, char *destEid, char *reportToEid, int lifespan, int classOfService, BpCustodySwitch custodySwitch, unsigned char srrFlags, int ackRequested, BpExtendedCOS *extendedCOS, Object adu, Object *newBundle);
typedef int (*bp_receive_t)(BpSAP sap, BpDelivery *dlvBuffer, int timeoutSeconds);
typedef int (*bp_add_endpoint_t)(char *eid, char *script);
typedef void (*bp_release_delivery_t)(BpDelivery *dlvBuffer, int releaseAdu);

typedef Object (*Sdr_malloc_t)(char *file, int line, Sdr sdr, unsigned long size);
typedef void (*sdr_begin_xn_t)(Sdr sdr);
typedef void (*sdr_cancel_xn_t)(Sdr sdr);
typedef int (*sdr_end_xn_t)(Sdr sdr);
typedef void (*Sdr_write_t)(char *file, int line, Sdr sdr, Address into, char *from, int length);

typedef Object (*zco_create_t)(Sdr sdr, ZcoMedium firstExtentSourceMedium, Object firstExtentLocation, unsigned int firstExtentOffset, unsigned int firstExtentLength);
typedef int (*zco_append_extent_t)(Sdr sdr, Object zcoRef, ZcoMedium sourceMedium, Object location, unsigned int offset, unsigned int length);
typedef void (*zco_start_receiving_t)(Sdr sdr, Object zcoRef, ZcoReader *reader);
typedef int (*zco_receive_source_t)(Sdr sdr, ZcoReader *reader, unsigned int length, char *buffer);
typedef void (*zco_stop_receiving_t)(Sdr sdr, ZcoReader *reader);

typedef void (*writeErrMemo_t)(char *);
typedef void (*writeErrmsgMemos_t)(void);

typedef struct {
    void (*sm_set_basekey)(unsigned int key);
    int (*bp_attach)(void);
    void (*bp_interrupt)(BpSAP sap);
    int (*bp_open)(char *eid, BpSAP *ionsapPtr);
    void (*bp_close)(BpSAP sap);
    Sdr (*bp_get_sdr)(void);
    int (*bp_send)(BpSAP sap, int mode, char *destEid, char *reportToEid, int lifespan, int classOfService, BpCustodySwitch custodySwitch, unsigned char srrFlags, int ackRequested, BpExtendedCOS *extendedCOS, Object adu, Object *newBundle);
    int (*bp_receive)(BpSAP sap, BpDelivery *dlvBuffer, int timeoutSeconds);
    int (*bp_add_endpoint)(char *eid, char *script);
    void (*bp_release_delivery)(BpDelivery *dlvBuffer, int releaseAdu);

    Object (*Sdr_malloc)(char *file, int line, Sdr sdr, unsigned long size);
    void (*sdr_begin_xn)(Sdr sdr);
    void (*sdr_cancel_xn)(Sdr sdr);
    int (*sdr_end_xn)(Sdr sdr);
    void (*Sdr_write)(char *file, int line, Sdr sdr, Address into, char *from, int length);

    Object (*zco_create)(Sdr sdr, ZcoMedium firstExtentSourceMedium, Object firstExtentLocation, unsigned int firstExtentOffset, unsigned int firstExtentLength);
    int (*zco_append_extent)(Sdr sdr, Object zcoRef, ZcoMedium sourceMedium, Object location, unsigned int offset, unsigned int length);
    void (*zco_start_receiving)(Sdr sdr, Object zcoRef, ZcoReader *reader);
    int (*zco_receive_source)(Sdr sdr, ZcoReader *reader, unsigned int length, char *buffer);
    void (*zco_stop_receiving)(Sdr sdr, ZcoReader *reader);

    void (*writeErrMemo)(char *);
    void (*writeErrmsgMemos)(void);
} bdm_bp_funcs_t;

#define bdm_sdr_malloc(sdr, size)            (*bdm_bp_funcs.Sdr_malloc)(__FILE__, __LINE__, sdr, size)
#define bdm_sdr_write(sdr, into, from, size) (*bdm_bp_funcs.Sdr_write)(__FILE__, __LINE__, sdr, into, from, size)

extern bdm_bp_funcs_t bdm_bp_funcs;
#endif


#include <sqlite3.h>

// Number of bytes to use for the resource key
// from the sha1 hash. Must be <= to SHA_DIGEST_LENGTH;
#define BDM_RESOURCE_KEY_LENGTH 8 

//
// bdm server listens on this TCP port, clients connect
//

#define BDM_PORT      (11002)
#define BDM_SYNC_PORT (11003)


extern void * libbdm_cal_handle;
extern bionet_bdm_t * this_bdm;
extern char *bdm_pidfile;
extern GMainLoop *bdm_main_loop;

extern GHashTable * bdm_opts_table;


// Global flag. Set true to stop all auxillary threads
extern int bdm_shutdown_now;

// Global CAL vars
extern int libbdm_cal_fd;
extern void bdm_cal_callback(void * cal_handle, const cal_event_t *event);
extern int libbdm_cal_topic_matches(const char * topic, const char *subscription);

// Global BDM Statistics
extern uint32_t num_bionet_datapoints;
extern uint32_t num_sync_datapoints;
extern uint32_t num_db_commits;

// Call from threads instead of sleep after starting a g_main_loop.
// Returns if thread should exit
int bdm_thread_sleep();

typedef enum {
    DB_INT = 0,
    DB_DOUBLE,
    DB_STRING
} db_type_t;

typedef union {
    int i;
    double d;
    char * str;
} db_value_t;

typedef struct {
    struct timeval timestamp;
    db_type_t type;
    db_value_t value;
    sqlite_int64 rowid;

    uint8_t node_uid[BDM_RESOURCE_KEY_LENGTH];
    uint8_t res_uid[BDM_RESOURCE_KEY_LENGTH];
} bdm_datapoint_t;

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
    int bundle_lifetime; // Sync bundles have this rfc5050 lifetime (seconds)

    //State vars
    sqlite3 *db;
    int last_entry_end_seq;    
    // TCP Specific
    int fd;
    int bytes_sent;
} sync_sender_config_t;

typedef struct {
    GList *event_list; // List of dbb_event_t to insert
    GData *bdm_list; // Cache of dbb_bdm_t to reference, and cleanup
    GData *hab_list; // Cache of dbb_hab_t to reference and cleanup

    // Set after a successfull flush.
    sqlite_int64 first_seq;
    sqlite_int64 last_seq;
} bdm_db_batch_t;


// 
// interface to the database backend
//


//
// Main thread db handle. Used by bionet callbacks
//
extern sqlite3 * main_db;
extern bdm_db_batch_t * dbb;

/**
 * Type of recorded event
 */
typedef enum {
    DBB_NEW_HAB_EVENT,
    DBB_LOST_HAB_EVENT,
    DBB_NEW_NODE_EVENT,
    DBB_LOST_NODE_EVENT,
    DBB_DATAPOINT_EVENT,
} dbb_event_type_t;

/**
 * Data pointers for all possible event types
 */
typedef union {
    bionet_hab_t * hab;
    bionet_node_t * node;
    bionet_datapoint_t * datapoint;
} dbb_bionet_event_data_t;

int datapoint_bionet_to_bdm(
    bionet_datapoint_t * datapoint,
    bdm_datapoint_t *dp);

bionet_datapoint_t * datapoint_bdm_to_bionet(
    bdm_datapoint_t *dp,
    bionet_resource_t * resource);



typedef struct dbb_event dbb_event_t;
typedef struct dbb_bdm dbb_bdm_t;
typedef struct dbb_hab dbb_hab_t;
typedef struct dbb_node dbb_node_t;
typedef struct dbb_resource dbb_resource_t;

bionet_node_t * node_bdm_to_bionet(dbb_node_t * dbb_node, bionet_hab_t * hab);

typedef union {
    dbb_hab_t * hab;
    dbb_node_t * node;
    struct {
        dbb_resource_t * resource;
        bdm_datapoint_t * dp;
    } datapoint;
} dbb_event_data_t;

struct dbb_event {
    dbb_bdm_t * recording_bdm;
    struct timeval timestamp;
    dbb_event_type_t type;
    dbb_event_data_t data;
    sqlite_int64 rowid;
};

struct dbb_bdm {
    sqlite_int64 rowid;
    char * bdm_id;
    GData * hab_list;
};

struct dbb_hab {
    sqlite_int64 rowid;
    char * hab_id;
    char * hab_type;
    GData * node_list;
};

struct dbb_node {
    sqlite_int64 rowid;
    struct dbb_hab * hab;
    char * node_id;
    uint8_t guid[BDM_RESOURCE_KEY_LENGTH];
    GData * resource_list;
};

struct dbb_resource {
    sqlite_int64 rowid;
    struct dbb_node * node;
    char * resource_id;
    bionet_resource_data_type_t data_type;
    bionet_resource_flavor_t flavor;
    uint8_t resource_key[BDM_RESOURCE_KEY_LENGTH];
    bionet_resource_t * bionet_resource;

    GSList *datapoint_list;
    int datapoint_list_reversed; // Set to true when the list is 'reversed' when flushing
};



dbb_event_t * dbb_add_event(
        bdm_db_batch_t *dbb,
        dbb_event_type_t type,
        dbb_bionet_event_data_t bionet_ptr,
        const char * bdm_id,
        const struct timeval *timestamp);

int dbb_flush_to_db(bdm_db_batch_t * dbb); // no-op when batch empty


//
// Free the list returned from db_get_metadata or db_get_resource_datapoints
//
void bdm_list_free(GPtrArray *hab_list);
void hab_list_free(GPtrArray *hab_list);

int db_get_latest_entry_seq(sqlite3 *db);

//
// Stuff to handle messages and asn
//
void libbdm_handle_resourceDatapointsQuery(
        const char * peer_name, 
        ResourceDatapointsQuery_t *rdpq);

BDM_Sync_Message_t * bdm_sync_metadata_to_asn(GPtrArray *hab_list);
BDM_Sync_Message_t * bdm_sync_datapoints_to_asn(GPtrArray *hab_list);


int bdm_report_datapoint(
        dbb_event_t * event,
        bionet_event_t * bionet_event);

int bdm_report_lost_node(
        dbb_event_t * event,
        bionet_event_t * bionet_event);

int bdm_report_new_node(
        dbb_event_t * event,
        bionet_event_t * bionet_event);

int bdm_report_new_hab(
        dbb_event_t * event,
        bionet_event_t * bionet_event);

int bdm_report_lost_hab(
        dbb_event_t * event,
        bionet_event_t * bionet_event);

int db_publish_synced_datapoints(
        sqlite3 *db,
        sqlite_int64 first_seq, 
        sqlite_int64 last_seq);

int db_publish_sync_affected_datapoints(
        sqlite3 *db,
        sqlite_int64 first_seq, 
        sqlite_int64 last_seq);

void dbb_free(bdm_db_batch_t * dbb);


// 
// stuff for being a bionet client
//

#define MAX_SUBSCRIPTIONS 100
extern int hab_list_index;
extern gchar ** hab_list_name_patterns;

extern int node_list_index;
extern gchar ** node_list_name_patterns;

extern int resource_index;
extern gchar ** resource_name_patterns;

extern char * security_dir;
extern int require_security;

int try_to_connect_to_bionet(void *unused);
void disconnect_from_bionet(void *unused);




// 
// stuff to actually be BDM
//


typedef struct {
    int fd;
    GIOChannel *ch;

#if ENABLE_ION
    struct {
        int	running;
        BpSAP	sap;
        Sdr		sdr;
	ZcoReader	reader;
    } ion;
#endif

    // keep track of messages coming in from the client
    union {
	BDM_C2S_Message_t             *C2S_message;
	BDM_Sync_Message_t            *sync_message;
    } message;
    unsigned char buffer[(10 * 1024)];  // FIXME: use cal
    int index;
} client_t;


#ifdef ENABLE_ION
extern client_t dtn_thread_data;
extern char * dtn_endpoint_id;
#endif

int client_connecting_handler(GIOChannel *ch, GIOCondition condition, gpointer listening_fd_as_pointer);
int client_readable_handler(GIOChannel *unused, GIOCondition unused2, client_t *client);
void handle_client_message(client_t *client, BDM_C2S_Message_t *message);
void disconnect_client(client_t *client);
void disconnect_sync_sender(client_t *client);

int sync_receive_connecting_handler(GIOChannel *ch, GIOCondition condition, gpointer listening_fd_as_pointer);
int sync_receive_readable_handler(GIOChannel *unused, GIOCondition unused2, client_t *client);
void handle_sync_datapoints_message(client_t *client, BDM_Sync_Datapoints_Message_t *message);
void handle_sync_metadata_message(client_t *client, BDM_Sync_Metadata_Message_t *message);



int make_listening_socket(int port);
void keepalive(int socket, int idle, int count, int interval);
void make_shutdowns_clean(int withThreads);


// BDM sync'ing
gpointer sync_thread(gpointer config_list);
gpointer dtn_receive_thread(gpointer config);

sync_sender_config_t * read_config_file(const char * fname);

void sync_sender_config_destroy(sync_sender_config_t * cfg);

#endif //  BIONET_DATA_MANAGER_H

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
