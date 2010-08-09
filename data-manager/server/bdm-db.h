

// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#ifndef BIONET_DATA_MANAGER_DB_H
#define BIONET_DATA_MANAGER_DB_H

#include "bionet-data-manager.h"

#define BDM_SQL_LOG_DOMAIN "bdm-sql"

#define UUID_FMTSTR "%02x%02x%02x%02x%02x%02x%02x%02x"
#define UUID_ARGS(x) x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7] 

//
// set up the database
//
// return 0 on success, -1 on failure
//
sqlite3 *db_init(const char * database_file);


// 
// shut down the database
//

void db_shutdown(sqlite3 *db);
int db_begin_transaction(sqlite3 *db);
int db_commit(sqlite3 *db);
void db_rollback(sqlite3 *db);


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

int db_add_datapoint(sqlite3 *db, bionet_datapoint_t *datapoint);
int db_add_node(sqlite3 *db, bionet_node_t *node);
int db_add_hab(sqlite3 *db, bionet_hab_t *hab);
int db_add_bdm(sqlite3 *db, const char * bdm_id);



int db_get_last_sync_seq(sqlite3 *db, char * bdm_id);
void db_set_last_sync_seq(sqlite3 *db, char * bdm_id, int last_sync);



typedef enum {
    BDM_HANDLE_OK = 0,
    BDM_HANDLE_STOP,
    BDM_HANDLE_ERR
} bdm_handle_row_status_t;

typedef enum {
    _DB_GET_HAB_EVENTS,
    _DB_GET_NODE_EVENTS,
    _DB_GET_DP_EVENTS
} db_get_event_class_t;

/**
 * Function pointer that gets called with every row
 *
 * Call _sql_value_to_bionet(stmt, value_idx,...) to obtain the formatted value once the
 * resource for it is known 
 */
typedef bdm_handle_row_status_t (*db_get_events_cb_t)(
        const char * bdm_id,
        const char * hab_type,
        const char * hab_id,
        const char * node_id, const uint8_t node_guid[BDM_UUID_LEN],
        bionet_resource_data_type_t datatype,
        bionet_resource_flavor_t flavor,
        const char * resource_id,
        const struct timeval *dp_timestamp,
        db_get_event_class_t event_class,
        bionet_event_type_t event_type,
        const struct timeval *event_timestamp,
        sqlite_int64 seq,
        const int column_idx,
        sqlite3_stmt * stmt,
        void * usr_data);

// 
// Finds all matching datapoints, and returns them as a GPtrArray of bionet_hab_t.
// Use bdm_list_free to free the list returned
//
GPtrArray *db_get_resource_datapoints(
    sqlite3 *db,
    const char *bdm_id,
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const char *resource_id,
    const struct timeval *datapoint_start,
    const struct timeval *datapoint_end,
    const struct timeval *event_start,
    const struct timeval *event_end,
    int entry_start,
    int entry_end
);

GPtrArray *db_get_resource_datapoints_bdm_list(
    sqlite3 *db,
    const char *bdm_id,
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const char *resource_id,
    const struct timeval *datapoint_start,
    const struct timeval *datapoint_end,
    const struct timeval *event_start,
    const struct timeval *event_end,
    int entry_start,
    int entry_end
);

// 
// Finds all matching metadata, and returns them as a GPtrArray of bionet_bdm_t.
// Use bdm_list_free to free the list returned
//
GPtrArray *db_get_metadata(
    sqlite3 *db,
    const char *bdm_id,
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const char *resource_id,
    const struct timeval *event_start, 
    const struct timeval *event_end,
    int entry_start,
    int entry_end);

// Return the metadata rooted at the recording BDM list
GPtrArray *db_get_metadata_bdmlist(
    sqlite3 *db,
    const char *bdm_id,
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const char *resource_id,
    const struct timeval *event_start,
    const struct timeval *event_end,
    int entry_start,
    int entry_end);

// Return all metadata and datapoints in a BDM list
GPtrArray *db_get_bdmlist(
    sqlite3 *db,
    const char *bdm_id,
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const char *resource_id,
    const struct timeval *event_start,
    const struct timeval *event_end,
    int entry_start,
    int entry_end);

GPtrArray *db_get_habs(
    sqlite3 *db,
    const char *bdm_id,
    const char *hab_type,
    const char *hab_id,
    const struct timeval *event_start, 
    const struct timeval *event_end,
    int entry_start,
    int entry_end);

GPtrArray *db_get_nodes(
    sqlite3 *db,
    const char *bdm_id,
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const struct timeval *event_start, 
    const struct timeval *event_end,
    int entry_start,
    int entry_end);

// Basic insert wrapper. No consistancy checking or transactions...
// All of them will return the inserted table key by reference if rowid pointer is non-NULL
// They all return < 0 on error, 0 if inserted, and 1 if already exists
int db_insert_bdm(
        sqlite3* db,
        const char *bdm_id,
        sqlite_int64 *rowid) ;
int db_insert_hab(sqlite3* db, const char * hab_type, const char * hab_id, sqlite_int64 *rowid);
int db_insert_node(
        sqlite3* db,
        const char * node_id,
        const char * hab_type,
        const char * hab_id,
        uint8_t guid[BDM_UUID_LEN],
        sqlite_int64 *rowid);

int db_insert_resource(
        sqlite3* db,
        const char * hab_type,
        const char * hab_id,
        const char * node_id,
        uint8_t node_guid[BDM_UUID_LEN],
        const char * resource_id,
        bionet_resource_flavor_t flavor,
        bionet_resource_data_type_t data_type);

int db_insert_datapoint(sqlite3* db, 
    uint8_t resource_key[BDM_RESOURCE_KEY_LENGTH],
    bdm_datapoint_t *dp,
    sqlite_int64 *rowid);

int db_insert_event(
    sqlite3 * db,
    const struct timeval *timestamp,
    sqlite_int64 bdm_row,
    dbb_event_type_t event_type,
    sqlite_int64 data_row,
    sqlite_int64 *rowid);


#endif /* BIONET_DATA_MANAGER_DB_H */

