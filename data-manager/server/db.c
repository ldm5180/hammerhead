
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#define _XOPEN_SOURCE 600
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sqlite3.h>
#include <openssl/sha.h>
#include <sys/time.h>
#include <glib.h>

#include <bionet.h>

#include "bdm-util.h"
#include "bionet-data-manager.h"


static int db_get_next_entry_seq(sqlite3 *db);
static int db_get_last_sync_seq(sqlite3 *db, char * bdm_id, sqlite3_stmt *stmt);
static void db_set_last_sync_seq(sqlite3 *db, char * bdm_id, int last_sync, sqlite3_stmt *stmt);


static int entry_seq = -1; // Always set before calling add_*_to_db

static sqlite3_stmt * insert_hab_stmt = NULL;
static sqlite3_stmt * insert_node_stmt = NULL;
static sqlite3_stmt * insert_resource_stmt = NULL;
static sqlite3_stmt * insert_datapoint_sync_stmt = NULL;
static sqlite3_stmt * insert_bdm_stmt = NULL;
static sqlite3_stmt * get_last_sync_bdm_stmt_metadata = NULL;
static sqlite3_stmt * get_last_sync_bdm_stmt_datapoints = NULL;
static sqlite3_stmt * set_last_sync_bdm_stmt_metadata = NULL;
static sqlite3_stmt * set_last_sync_bdm_stmt_datapoints = NULL;

extern char * database_file;
char bdm_id[256] = { 0 };

static int _datapoint_bionet_to_bdm(
    bionet_datapoint_t * datapoint,
    bdm_datapoint_t *dp,
    char * bdm_id) 
{

    bionet_resource_data_type_t type;
    bionet_value_t *value;
    int r = -1;

    type = bionet_resource_get_data_type(bionet_datapoint_get_resource(datapoint));
    value = bionet_datapoint_get_value(datapoint);

    dp->bdm_id = bdm_id;
    struct timeval * ts = bionet_datapoint_get_timestamp(datapoint);
    dp->timestamp.tv_sec = ts->tv_sec;
    dp->timestamp.tv_usec = ts->tv_usec;

    // Set type and value
    switch (type) {
        case BIONET_RESOURCE_DATA_TYPE_BINARY: {
            int i;
            r = bionet_value_get_binary(value, &i);
            dp->type = DB_INT;
            dp->value.i = i;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_UINT8: {
            uint8_t u8;
            dp->type = DB_INT;
            r = bionet_value_get_uint8(value, &u8);
            dp->value.i = u8;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_INT8: {
            int8_t i8;
            dp->type = DB_INT;
            r = bionet_value_get_int8(value, &i8);
            dp->value.i = i8;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_UINT16: {
            uint16_t u16;
            dp->type = DB_INT;
            r = bionet_value_get_uint16(value, &u16);
            dp->value.i = u16;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_INT16: {
            int16_t i16;
            dp->type = DB_INT;
            r = bionet_value_get_int16(value, &i16);
            dp->value.i = i16;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_UINT32: {
	    uint32_t u32;
            dp->type = DB_INT;
	    r = bionet_value_get_uint32(value, &u32);
            dp->value.i = u32;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_INT32: {
            int32_t i32;
            dp->type = DB_INT;
            r = bionet_value_get_int32(value, &i32);
            dp->value.i = i32;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_FLOAT: {
	    float f;
            r = bionet_value_get_float(value, &f);
            dp->type = DB_DOUBLE;
	    dp->value.d = f;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_DOUBLE: {
	    double d;
            dp->type = DB_DOUBLE;
            r = bionet_value_get_double(value, &d);
	    dp->value.d = d;
            break;
        }
        case BIONET_RESOURCE_DATA_TYPE_STRING: {
            r = bionet_value_get_str(value, &dp->value.str);
            dp->type = DB_STRING;
            break;
        }
        default: {
            g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s(): invalid datatype %d", 
                __FUNCTION__, type);
            return -1;
        }
    }
    return r;
}

typedef struct sql_return {
    GPtrArray *bdm_list;
} sql_return_t;

sqlite3 *db_init(void) {
    int r;

    sqlite3 * db = NULL;

    r = sqlite3_open(database_file, &db);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error opening database %s: %s\n", database_file, sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    r = sqlite3_busy_timeout(db, 5 * 1000);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error setting busy timeout on database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    return db;
}


// 
// shut down the database
//

void db_shutdown(sqlite3 *db) {

    if(insert_hab_stmt){
	sqlite3_finalize(insert_hab_stmt);
	insert_hab_stmt = NULL;
    }
    if(insert_node_stmt){
	sqlite3_finalize(insert_node_stmt);
	insert_node_stmt = NULL;
    }
    if(insert_resource_stmt){
	sqlite3_finalize(insert_resource_stmt);
	insert_resource_stmt = NULL;
    }
    if(insert_datapoint_sync_stmt){
	sqlite3_finalize(insert_datapoint_sync_stmt);
	insert_datapoint_sync_stmt = NULL;
    }
    if(insert_bdm_stmt){
	sqlite3_finalize(insert_bdm_stmt);
	insert_bdm_stmt = NULL;
    }
    if(get_last_sync_bdm_stmt_datapoints){
	sqlite3_finalize(get_last_sync_bdm_stmt_datapoints);
	get_last_sync_bdm_stmt_datapoints = NULL;
    }
    if(get_last_sync_bdm_stmt_metadata){
	sqlite3_finalize(get_last_sync_bdm_stmt_metadata);
	get_last_sync_bdm_stmt_metadata = NULL;
    }
    if(set_last_sync_bdm_stmt_datapoints){
	sqlite3_finalize(set_last_sync_bdm_stmt_datapoints);
	set_last_sync_bdm_stmt_datapoints = NULL;
    }
    if(set_last_sync_bdm_stmt_metadata){
	sqlite3_finalize(set_last_sync_bdm_stmt_metadata);
	set_last_sync_bdm_stmt_metadata = NULL;
    }

    sqlite3_close(db);
}




// this calls COMMIT and retries indefinately if the DB is busy
static int do_commit(sqlite3 *db) {
    int r;
    char *zErrMsg = NULL;

    do {
        r = sqlite3_exec(
            db,
            "COMMIT;",
            NULL,
            0,
            &zErrMsg
        );

        if (r == SQLITE_OK) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
                  "committed(%p)", db);
             return 0;
        }

        if (r == SQLITE_BUSY) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "COMMIT failed because the database is busy (\"%s\"), retrying", zErrMsg);
            sqlite3_free(zErrMsg);
            g_usleep(20 * 1000);
            continue;
        }

        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "COMMIT SQL error: %s", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    } while (1);

}

static int do_begin_transaction(sqlite3 *db) {
    int r;
    char *zErrMsg = NULL;

    r = sqlite3_exec(
        db,
        "BEGIN TRANSACTION;",
        NULL,
        0,
        &zErrMsg
    );
    if (r != SQLITE_OK) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "BEGIN TRANSACTION SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	  "in transaction(%p)", db);

    return 0;
}

// 
// database back-end internal helper functions
//
// Each of these functions does a single SQL INSERT.
// Returns 0 on success, -1 on failure.
//


static int add_hab_to_db(sqlite3* db, const bionet_hab_t *hab) {
    int r;

    if(insert_hab_stmt == NULL) {
	r = sqlite3_prepare_v2(db, 
            "INSERT"
            " OR IGNORE"
            " INTO Hardware_Abstractors (HAB_Type, HAB_ID, Entry_Num) "
            " VALUES (?,?,?)",
	    -1, &insert_hab_stmt, NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-hab SQL error: %s\n", sqlite3_errmsg(db));
	    return -1;
	}
    }

    int param = 1;
    r = sqlite3_bind_text(insert_hab_stmt, param++, bionet_hab_get_type(hab), -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-hab SQL bind error");
	return -1;
    }

    r = sqlite3_bind_text(insert_hab_stmt, param++, bionet_hab_get_id(hab), -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-hab SQL bind error");
	return -1;
    }
    r = sqlite3_bind_int(insert_hab_stmt, param++, entry_seq);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-hab SQL bind error");
	return -1;
    }

    while(SQLITE_BUSY == (r = sqlite3_step(insert_hab_stmt))){
        g_usleep(20 * 1000);
    }
        
    if (r != SQLITE_DONE) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-hab SQL error: %s\n", sqlite3_errmsg(db));
	sqlite3_reset(insert_hab_stmt);
        return -1;
    }
    sqlite3_reset(insert_hab_stmt);
    sqlite3_clear_bindings(insert_hab_stmt);

    return 0;
}




static int add_node_to_db(sqlite3* db, const bionet_node_t *node) {
    int r;

    if(insert_node_stmt == NULL) {
	r = sqlite3_prepare_v2(db, 
            "INSERT"
            " OR IGNORE"
            " INTO Nodes (HAB_Key, Node_ID, Entry_Num)"
            " SELECT"
            "     Hardware_Abstractors.Key, ?, ?"
            "     FROM Hardware_Abstractors"
            "     WHERE"
            "         HAB_Type=?"
            "         AND HAB_ID=?",
            -1, &insert_node_stmt, NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-node SQL error: %s\n", sqlite3_errmsg(db));
	    return -1;
	}
    }


    int param = 1;
    r = sqlite3_bind_text(insert_node_stmt, param++, bionet_node_get_id(node), -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-node SQL bind error");
	return -1;
    }

    r = sqlite3_bind_int( insert_node_stmt, param++,  entry_seq);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-node SQL bind error");
	return -1;
    }

    r = sqlite3_bind_text(insert_node_stmt, param++, bionet_hab_get_type(bionet_node_get_hab(node)), -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-node SQL bind error");
	return -1;
    }

    r = sqlite3_bind_text(insert_node_stmt, param++, bionet_hab_get_id(bionet_node_get_hab(node)), -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-node SQL bind error");
	return -1;
    }

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	  "add_node_to_db(%p): [%d] %s", 
          db,
          entry_seq,
          bionet_node_get_name(node));

    while(SQLITE_BUSY == (r = sqlite3_step(insert_node_stmt))){
        g_usleep(20 * 1000);
    }
    if (r != SQLITE_DONE) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-node SQL error: %s\n", sqlite3_errmsg(db));
	sqlite3_reset(insert_node_stmt);
        return -1;
    }
    sqlite3_reset(insert_node_stmt);
    sqlite3_clear_bindings(insert_node_stmt);

    return 0;
}


int db_make_resource_key(
    const char * hab_type,
    const char * hab_id,
    const char * node_id,
    const char * resource_id,
    bionet_resource_data_type_t data_type,
    bionet_resource_flavor_t flavor,
    uint8_t resource_key[BDM_RESOURCE_KEY_LENGTH]
){
    SHA_CTX sha_ctx;
    unsigned char sha_digest[SHA_DIGEST_LENGTH];
    int r;
    uint8_t byte;

    r = SHA1_Init(&sha_ctx);
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error initializing SHA1 context\n");
        return -1;
    }

    r = SHA1_Update(&sha_ctx, hab_type, strlen(hab_type));
    if (r != 1) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource HAB-Type\n");
	return -1;
    }
    
    r = SHA1_Update(&sha_ctx, hab_id, strlen(hab_id));
    if (r != 1) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource HAB-ID\n");
	return -1;
    }

    r = SHA1_Update(&sha_ctx, node_id, strlen(node_id));
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource Node-ID\n");
        return -1;
    }

    r = SHA1_Update(&sha_ctx, resource_id, strlen(resource_id));
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource ID\n");
        return -1;
    }

    byte = data_type;
    r = SHA1_Update(&sha_ctx, &byte, 1);
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource Data Type\n");
        return -1;
    }

    byte = flavor;
    r = SHA1_Update(&sha_ctx, &byte, 1);
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource Flavor\n");
        return -1;
    }

    r = SHA1_Final(sha_digest, &sha_ctx);
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error finalizing SHA1 digest\n");
        return -1;
    }

    memcpy(resource_key, sha_digest,BDM_RESOURCE_KEY_LENGTH);

    return 0;

}

static int add_resource_to_db(sqlite3* db, bionet_resource_t *resource) {
    int r;

    uint8_t resource_key[BDM_RESOURCE_KEY_LENGTH];

    const char * hab_type;
    const char * hab_id;
    const char * node_id;
    const char * resource_id;
    bionet_resource_flavor_t flavor;
    bionet_resource_data_type_t data_type;

    hab_type = bionet_hab_get_type(bionet_resource_get_hab(resource));
    hab_id = bionet_hab_get_id(bionet_resource_get_hab(resource));
    node_id = bionet_node_get_id(bionet_resource_get_node(resource));
    resource_id = bionet_resource_get_id(resource);
    data_type = bionet_resource_get_data_type(resource);
    flavor = bionet_resource_get_flavor(resource);

    r = db_make_resource_key(hab_type, hab_id, node_id, 
        resource_id, data_type, flavor, resource_key);
    if(r != 0){
        return r;
    }

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	  "add_resource_to_db(%p): [%d] %s: %2x%2x%2x%2x%2x%2x%2x%2x", 
          db,
          entry_seq,
          bionet_resource_get_name(resource), 
          (int)resource_key[0], (int)resource_key[1], 
          (int)resource_key[2], (int)resource_key[3],
          (int)resource_key[4], (int)resource_key[5],
          (int)resource_key[6], (int)resource_key[7]);

    if(insert_resource_stmt == NULL) {
	r = sqlite3_prepare_v2(db, 
            "INSERT"
            " OR IGNORE"
            " INTO Resources"
            " SELECT"
            "     ?, Nodes.Key, ?, Resource_Data_Types.Key, Resource_Flavors.Key, ?"
            "     FROM Hardware_Abstractors, Nodes, Resource_Data_Types, Resource_Flavors"
            "     WHERE"
            "         Hardware_Abstractors.HAB_Type=?"
            "         AND Hardware_Abstractors.HAB_ID=?"
            "         AND Nodes.HAB_Key=Hardware_Abstractors.Key"
            "         AND Nodes.Node_ID=?"
            "         AND Resource_Data_Types.Data_Type LIKE ?"
            "         AND Resource_Flavors.Flavor LIKE ?",
	    -1, &insert_resource_stmt, NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL error: %s\n", sqlite3_errmsg(db));
	    return -1;
	}
    }

    int param = 1;
    r = sqlite3_bind_blob(insert_resource_stmt, param++, resource_key, BDM_RESOURCE_KEY_LENGTH, SQLITE_TRANSIENT);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL bind error");
	return -1;
    }
    r = sqlite3_bind_text(insert_resource_stmt, param++, resource_id, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL bind error");
	return -1;
    }
    r = sqlite3_bind_int( insert_resource_stmt, param++,  entry_seq);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL bind error");
	return -1;
    }
    r = sqlite3_bind_text(insert_resource_stmt, param++, hab_type, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL bind error");
	return -1;
    }
    r = sqlite3_bind_text(insert_resource_stmt, param++, hab_id, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL bind error");
	return -1;
    }
    r = sqlite3_bind_text(insert_resource_stmt, param++, node_id, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL bind error");
	return -1;
    }
    r = sqlite3_bind_text(insert_resource_stmt, param++, bionet_resource_data_type_to_string(data_type), -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL bind error");
	return -1;
    }
    r = sqlite3_bind_text(insert_resource_stmt, param++, bionet_resource_flavor_to_string(flavor), -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL bind error");
	return -1;
    }

    while(SQLITE_BUSY == (r = sqlite3_step(insert_resource_stmt))){
        g_usleep(20 * 1000);
    }
    if (r != SQLITE_DONE) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL error: %s\n", sqlite3_errmsg(db));
	sqlite3_reset(insert_resource_stmt);
        return -1;
    }
    sqlite3_reset(insert_resource_stmt);
    sqlite3_clear_bindings(insert_resource_stmt);

    return 0;
}


int db_add_bdm(sqlite3* db, const char *bdm_id) {
    int r;

    if(insert_bdm_stmt == NULL) {
	r = sqlite3_prepare_v2(db, 
	    "INSERT"
	    " OR IGNORE"
	    " INTO BDMs(BDM_ID) VALUES(?)",
	    -1, &insert_bdm_stmt, NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-bdm SQL error: %s\n", sqlite3_errmsg(db));
	    return -1;
	}
    }

    int param = 1;
    r = sqlite3_bind_text(insert_bdm_stmt, param++, bdm_id, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-bdm SQL bind error");
	return -1;
    }

    while(SQLITE_BUSY == (r = sqlite3_step(insert_bdm_stmt))){
        g_usleep(20 * 1000);
    }
    if (r != SQLITE_DONE) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-bdm SQL error: %s\n", sqlite3_errmsg(db));
	sqlite3_reset(insert_bdm_stmt);
        return -1;
    }
    sqlite3_reset(insert_bdm_stmt);
    sqlite3_clear_bindings(insert_bdm_stmt);

    return 0;
}


/*
 * Add the datapoint to the DB. 
 * Does NOT require metadata be inserted first
 */
static int add_datapoint_to_db(sqlite3* db, 
    uint8_t resource_key[BDM_RESOURCE_KEY_LENGTH],
    bdm_datapoint_t *dp)
{
    int r;

    if(insert_datapoint_sync_stmt == NULL) {
	r = sqlite3_prepare_v2(db, 
	    "INSERT"
	    " OR IGNORE"
	    " INTO Datapoints"
	    " SELECT"
	    "     NULL, ?, BDMs.Key, ?, ?, ?, ?"
	    "     FROM BDMs"
	    "     WHERE BDMs.BDM_ID = ?",
	    -1, &insert_datapoint_sync_stmt, NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add_datapoint_to_db SQL error: %s\n", sqlite3_errmsg(db));
	    return -1;
	}
    }

    // Bind host variables to the prepared statement 
    // -- This eliminates the need to escape strings
    // Bind in order of the placeholders (?) in the SQL
    int param = 1;
    r = sqlite3_bind_blob(insert_datapoint_sync_stmt, param++, 
        resource_key, BDM_RESOURCE_KEY_LENGTH, SQLITE_TRANSIENT);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add_datapoint_to_db SQL bind error: %d", param);
	return -1;
    }

    switch(dp->type) {
        case DB_INT:
            r = sqlite3_bind_int(insert_datapoint_sync_stmt, param++, dp->value.i);
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
		  "%s(): (unsigned int)%lu (int)%li", __FUNCTION__, 
                  (unsigned long)dp->value.i, (long)dp->value.i);
            break;
            
        case DB_DOUBLE:
	    r = sqlite3_bind_double(insert_datapoint_sync_stmt, param++, dp->value.d);
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
		  "%s(): (double)%.16g", __FUNCTION__, dp->value.d);
            break;
            
        case DB_STRING:
            r = sqlite3_bind_text(insert_datapoint_sync_stmt, param++, dp->value.str, -1, SQLITE_STATIC);
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
		  "%s(): (str)%s", __FUNCTION__, dp->value.str);
            break;

        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add_datapoint_to_db internal error: type %d invalid", dp->type);
            return -1;

    }
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add_datapoint_to_db SQL bind error: %d", param);
	return -1;
    }

    r = sqlite3_bind_int( insert_datapoint_sync_stmt, param++,  dp->timestamp.tv_sec);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add_datapoint_to_db SQL bind error: %d", param);
	return -1;
    }

    r = sqlite3_bind_int( insert_datapoint_sync_stmt, param++,  dp->timestamp.tv_usec);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add_datapoint_to_db SQL bind error: %d", param);
	return -1;
    }

    r = sqlite3_bind_int( insert_datapoint_sync_stmt, param++,  entry_seq);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add_datapoint_to_db SQL bind error: %d", param);
	return -1;
    }

    r = sqlite3_bind_text(insert_datapoint_sync_stmt, param++, dp->bdm_id, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add_datapoint_to_db SQL bind error: %d", param);
	return -1;
    }

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	  "add_datapoint_to_db(%p) [%d]: %2x%2x%2x%2x%2x%2x%2x%2x", 
          db,
          entry_seq,
          (int)resource_key[0], (int)resource_key[1], 
          (int)resource_key[2], (int)resource_key[3],
          (int)resource_key[4], (int)resource_key[5],
          (int)resource_key[6], (int)resource_key[7]);


    while(SQLITE_BUSY == (r = sqlite3_step(insert_datapoint_sync_stmt))){
        g_usleep(20 * 1000);
    }
    if (r != SQLITE_DONE) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add_datapoint_to_db SQL error: %s\n", sqlite3_errmsg(db));
	sqlite3_reset(insert_datapoint_sync_stmt);
        return -1;
    }
    sqlite3_reset(insert_datapoint_sync_stmt);
    sqlite3_clear_bindings(insert_datapoint_sync_stmt);

    return 0;
}

//
// this is the true database interface for the bdm front-end to use, 
// and for the bdm back-end to provide
//

int db_add_datapoint(sqlite3* db, bionet_datapoint_t *datapoint) {
    int r;
    char *zErrMsg = NULL;
    bionet_value_t * value = NULL;
    bionet_resource_t * resource = NULL;
    bionet_node_t * node = NULL;
    bionet_hab_t * hab = NULL;
    uint8_t resource_key[BDM_RESOURCE_KEY_LENGTH];
    bdm_datapoint_t dp;

    entry_seq = db_get_next_entry_seq(db);
    if(entry_seq < 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            "Error getting sequence number\n");
        return -1;
    }

    // start transaction
    r = do_begin_transaction(db);
    if (r != 0) return -1;


    // add parent objects as needed
    value = bionet_datapoint_get_value(datapoint);
    resource = bionet_value_get_resource(value);
    node = bionet_resource_get_node(resource);
    hab = bionet_node_get_hab(node);

    r = add_hab_to_db(db, hab);
    if (r != 0) goto fail;

    r = add_node_to_db(db, node);
    if (r != 0) goto fail;

    r = add_resource_to_db(db, resource);
    if (r != 0) goto fail;

    // now finally add the data point itself
    db_make_resource_key(
        bionet_hab_get_type(hab),
        bionet_hab_get_id(hab),
        bionet_node_get_id(node),
        bionet_resource_get_id(resource),
        bionet_resource_get_data_type(resource),
        bionet_resource_get_flavor(resource),
        resource_key);

    r = _datapoint_bionet_to_bdm(datapoint, &dp, bdm_id);
    if (r != 0) goto fail;

    r = add_datapoint_to_db(db, resource_key, &dp); 
    if (r != 0) goto fail;


    // it all worked, commit it to the DB
    r = do_commit(db);
    if (r != 0) goto fail;

    return 0;


fail:
    r = sqlite3_exec(
        db,
        "ROLLBACK;",
        NULL,
        0,
        &zErrMsg
    );
    if (r != SQLITE_OK) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "ROLLBACK SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    return -1;
}


//
// this is the true database interface for the bdm-sync reciever to use.
// This allows storing datapoints before the metadata has arrived
//

int db_add_datapoint_sync(
    sqlite3 *db,
    uint8_t resource_key[BDM_RESOURCE_KEY_LENGTH],
    bdm_datapoint_t * dp)
{
    int r;

    r = db_add_bdm(db, dp->bdm_id);
    if ( r != 0 ){
        return r;
    }

    entry_seq = db_get_next_entry_seq(db);
    if(entry_seq < 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            "Error getting sequence number\n");
        return -1;
    }

    // Single insert, so no transaction needed
    return add_datapoint_to_db(db, resource_key, dp);
}


int db_add_node(sqlite3* db, bionet_node_t *node) {
    int i;

    int r;
    char *zErrMsg = NULL;

    bionet_hab_t * hab = NULL;

    hab = bionet_node_get_hab(node);

    entry_seq = db_get_next_entry_seq(db);
    if(entry_seq < 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            "Error getting sequence number\n");
        return -1;
    }


    // start transaction
    r = do_begin_transaction(db);
    if (r != 0) return -1;


    // add parent hab
    r = add_hab_to_db(db, hab);
    if (r != 0) goto fail;


    // add this node
    r = add_node_to_db(db, node);
    if (r != 0) goto fail;


    // add this node's resources
    for (i = 0; i < bionet_node_get_num_resources(node); i++) {
        bionet_resource_t *resource = bionet_node_get_resource_by_index(node, i);
        bionet_datapoint_t *d = bionet_resource_get_datapoint_by_index(resource, 0);

        r = add_resource_to_db(db, resource);
        if (r != 0) goto fail;

        // add the resource's data point, if any
        if (d != NULL) {
            bdm_datapoint_t dp;
            uint8_t resource_key[BDM_RESOURCE_KEY_LENGTH];

            db_make_resource_key(
                bionet_hab_get_type(hab),
                bionet_hab_get_id(hab),
                bionet_node_get_id(node),
                bionet_resource_get_id(resource),
                bionet_resource_get_data_type(resource),
                bionet_resource_get_flavor(resource),
                resource_key);

            r = _datapoint_bionet_to_bdm(d, &dp, bdm_id);
            if (r != 0) goto fail;

            r = add_datapoint_to_db(db, resource_key, &dp);
            if (r != 0) goto fail;
        }
    }


    // it all worked, commit it to the DB
    r = do_commit(db);
    if (r != 0) goto fail;

    return 0;


fail:
    r = sqlite3_exec(
        db,
        "ROLLBACK;",
        NULL,
        0,
        &zErrMsg
    );
    if (r != SQLITE_OK) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "ROLLBACK SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    return -1;
}




int db_add_hab(sqlite3* db, bionet_hab_t *hab) {
    int r;

    entry_seq = db_get_next_entry_seq(db);
    if(entry_seq < 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            "Error getting sequence number\n");
        return -1;
    }

    // this doesnt need a transaction because it's just a single INSERT
    r = add_hab_to_db(db, hab);
    if (r != 0) {
        return -1;
    }

    return 0;
}


void bdm_list_free(GPtrArray *bdm_list) {
    int b, h;
    for (b = 0; b < bdm_list->len; b++) {
        bdm_t * bdm = g_ptr_array_index(bdm_list, b);

        for (h = 0; h < bdm->hab_list->len; h++) {
            bionet_hab_t * hab = g_ptr_array_index(bdm->hab_list, h);

            bionet_hab_free(hab);
        }

        free(bdm->bdm_id);
        g_ptr_array_free(bdm->hab_list, TRUE);
        free(bdm);
    }
    g_ptr_array_free(bdm_list, TRUE);

}

static bdm_t *find_bdm(GPtrArray *bdm_list, const char *bdm_id) {
    int i;
    bdm_t *bdm;

    for (i = 0; i < bdm_list->len; i ++) {
        bdm = g_ptr_array_index(bdm_list, i);
	if ( strcmp(bdm->bdm_id, bdm_id) == 0) {
            return bdm;
        }
    }

    //
    // the requested bdm is not in the list, so add it
    //

    bdm = malloc(sizeof(bdm_t));
    if (bdm == NULL) return NULL;
    bdm->hab_list = g_ptr_array_new();
    if (bdm_id) {
	bdm->bdm_id = strdup(bdm_id);
    } else {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "find_hab(): BDM has no ID");
    } 
    g_ptr_array_add(bdm_list, bdm);
    return bdm;
}

static bionet_hab_t *find_hab(bdm_t * bdm, const char *hab_type, const char *hab_id) {
    int i;
    bionet_hab_t *hab;
    GPtrArray * hab_list = bdm->hab_list;

    for (i = 0; i < hab_list->len; i ++) {
        hab = g_ptr_array_index(hab_list, i);
        if ((strcmp(bionet_hab_get_type(hab), hab_type) == 0) 
	    && (strcmp(bionet_hab_get_id(hab), hab_id) == 0)) {
            return hab;
        }
    }

    //
    // the requested hab is not in the list, so add it
    //

    hab = bionet_hab_new(hab_type, hab_id);
    if (hab == NULL) return NULL;

    g_ptr_array_add(hab_list, hab);
    return hab;
}


static bionet_node_t *find_node(bionet_hab_t *hab, const char *node_id) {
    bionet_node_t *node;

    node = bionet_hab_get_node_by_id(hab, node_id);
    if (node != NULL) return node;

    node = bionet_node_new(hab, node_id);
    if (node == NULL) {
        return NULL;
    }

    if (bionet_hab_add_node(hab, node)) {
	g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "find_node(): Failed to add node to hab.");
	bionet_node_free(node);
	return NULL;
    }

    return node;
}


static bionet_resource_t *find_resource(bionet_node_t *node, const char *data_type, const char *flavor, const char *resource_id) {
    bionet_resource_t *resource;

    resource = bionet_node_get_resource_by_id(node, resource_id);
    if (resource != NULL) return resource;

    resource = bionet_resource_new(node, 
				   bionet_resource_data_type_from_string(data_type), 
				   bionet_resource_flavor_from_string(flavor), 
				   resource_id);
    if (resource == NULL) {
        return NULL;
    }

    if (bionet_node_add_resource(node, resource)) {
	g_log("", G_LOG_LEVEL_WARNING, "find_resource(): Failed to add resource to node.");
	bionet_resource_free(resource);
	return NULL;
    }

    return resource;
}


/*
 * Called once per sqlite3_step()
 */
static int _add_to_bdm_list(
    sqlite3_stmt *stmt,
    GPtrArray *bdm_list
) {
    bionet_hab_t *hab;
    bionet_node_t *node;
    bionet_resource_t *resource;

    struct timeval timestamp = {0,0};
    int err = 0;
    bionet_value_t * value = NULL;
    bionet_datapoint_t * datapoint = NULL;


    const char * habtype_id  = (const char *)sqlite3_column_text(stmt, 0); 
    const char * habname_id  = (const char *)sqlite3_column_text(stmt, 1); 
    const char * node_id     = (const char *)sqlite3_column_text(stmt, 2); 
    const char * datatype    = (const char *)sqlite3_column_text(stmt, 3); 
    const char * flavor      = (const char *)sqlite3_column_text(stmt, 4); 
    const char * resource_id = (const char *)sqlite3_column_text(stmt, 5); 
    static const int i_value =                                         6; 
    timestamp.tv_sec         =               sqlite3_column_int (stmt, 7);
    timestamp.tv_usec        =               sqlite3_column_int (stmt, 8);
    const char * bdm_id      = (const char *)sqlite3_column_text(stmt, 9); 
    
    bdm_t * bdm = find_bdm(bdm_list, bdm_id);
    if (bdm == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "db_get_resource_datapoints_callback(): error finding bdm %s", 
            bdm_id);
        return -1;
    }

    hab = find_hab(bdm, habtype_id, habname_id);
    if (hab == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "db_get_resource_datapoints_callback(): error finding hab %s.%s", habtype_id, habname_id);
        return -1;
    }

    node = find_node(hab, node_id);
    if (node == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
	      "db_get_resource_datapoints_callback(): error finding node %s.%s.%s", 
	      bionet_hab_get_type(hab), bionet_hab_get_id(hab), node_id);
        return -1;
    }

    resource = find_resource(node, datatype, flavor, resource_id);
    if (resource == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
	      "db_get_resource_datapoints_callback(): error finding resource %s %s %s.%s.%s:%s", 
	      datatype, flavor, 
	      bionet_hab_get_type(hab), bionet_hab_get_id(hab), 
	      bionet_node_get_id(node), resource_id);
        return -1;
    }

    int column_type = sqlite3_column_type(stmt, i_value);
    if(column_type != SQLITE_NULL){
        switch(bionet_resource_get_data_type(resource))
        {
        case BIONET_RESOURCE_DATA_TYPE_BINARY:
            value = bionet_value_new_binary(resource, 
                sqlite3_column_int(stmt, i_value));
            break; 

        case BIONET_RESOURCE_DATA_TYPE_UINT8:
            value = bionet_value_new_uint8(resource, 
                (uint8_t)sqlite3_column_int(stmt, i_value));
            break; 

        case BIONET_RESOURCE_DATA_TYPE_INT8:   
            value = bionet_value_new_int8(resource, 
                (int8_t)sqlite3_column_int(stmt, i_value));
            break; 

        case BIONET_RESOURCE_DATA_TYPE_UINT16: 
            value = bionet_value_new_uint16(resource, 
                (uint16_t)sqlite3_column_int(stmt, i_value));
            break; 
            
        case BIONET_RESOURCE_DATA_TYPE_INT16:  
            value = bionet_value_new_int16(resource, 
                (int16_t)sqlite3_column_int(stmt, i_value));
            break; 

        case BIONET_RESOURCE_DATA_TYPE_UINT32: 
            value = bionet_value_new_uint32(resource, 
                (uint32_t)sqlite3_column_int(stmt, i_value));
            break; 

        case BIONET_RESOURCE_DATA_TYPE_INT32:  
            value = bionet_value_new_int32(resource, 
                (int32_t)sqlite3_column_int(stmt, i_value));
            break; 

        case BIONET_RESOURCE_DATA_TYPE_FLOAT:  
            value = bionet_value_new_float(resource, 
                (float)sqlite3_column_double(stmt, i_value));
            break; 

        case BIONET_RESOURCE_DATA_TYPE_DOUBLE: 
            value = bionet_value_new_double(resource, 
                sqlite3_column_double(stmt, i_value));
            break; 

        case BIONET_RESOURCE_DATA_TYPE_STRING:
            value = bionet_value_new_str(resource, 
                (const char*)sqlite3_column_text(stmt, i_value));
            break; 

        case BIONET_RESOURCE_DATA_TYPE_INVALID:
        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
                  "db_get_resource_datapoints_callback(): invalid data_type %d for %s.%s.%s:%s\n",
                  bionet_resource_get_data_type(resource),
                  bionet_hab_get_type(hab),
                  bionet_hab_get_id(hab),
                  bionet_node_get_id(node),
                  bionet_resource_get_id(resource));
            break; 
        }

        if (value)
        {
            datapoint = bionet_datapoint_new(resource, value, &timestamp);
            if (datapoint) {
                bionet_resource_add_datapoint(resource, datapoint);
            } else {
                err++;
            }
        } else	{
            err++;
        }
    }

    if (err)
    {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
	      "db_get_resource_datapoints_callback(): error adding datapoint: %s.%s.%s:%s\n",
	      bionet_hab_get_type(hab),
	      bionet_hab_get_id(hab),
	      bionet_node_get_id(node),
	      bionet_resource_get_id(resource));
    }

    return 0;
}

// 
// Return data for the given filter parameters
//
static GPtrArray *_db_get_resource_info(sqlite3* db, 
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const char *resource_id,
    struct timeval *datapoint_start,
    struct timeval *datapoint_end,
    int entry_start,
    int entry_end,
    int include_datapoints) 
{
    int r;
    char sql[2048];

    char hab_type_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    char hab_id_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    char node_id_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    char resource_id_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    char datapoint_start_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    char datapoint_end_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    /* The size of these is mostly fixed */
    char entry_restriction[400];

    GPtrArray *bdm_list;

    bdm_list = g_ptr_array_new();

    if ((hab_type == NULL) || (strcmp(hab_type, "*") == 0)) {
        hab_type_restriction[0] = '\0';
    } else {
        r = snprintf(
            hab_type_restriction,
            sizeof(hab_type_restriction),
            "AND Hardware_Abstractors.HAB_Type = '%s'",
            hab_type
        );
        if (r >= sizeof(hab_type_restriction)) {
            g_log(
                BDM_LOG_DOMAIN,
                G_LOG_LEVEL_WARNING,
                "db_get_resource_datapoints(): hab type restriction too long!"
            );
            return NULL;
        }
    }


    if ((hab_id == NULL) || (strcmp(hab_id, "*") == 0)) {
        hab_id_restriction[0] = '\0';
    } else {
        r = snprintf(
            hab_id_restriction,
            sizeof(hab_id_restriction),
            "AND Hardware_Abstractors.HAB_ID = '%s'",
            hab_id
        );
        if (r >= sizeof(hab_id_restriction)) {
            g_log(
                BDM_LOG_DOMAIN,
                G_LOG_LEVEL_WARNING,
                "db_get_resource_datapoints(): hab id restriction too long!"
            );
            return NULL;
        }
    }


    if ((node_id == NULL) || (strcmp(node_id, "*") == 0)) {
        node_id_restriction[0] = '\0';
    } else {
        r = snprintf(
            node_id_restriction,
            sizeof(node_id_restriction),
            "AND Nodes.Node_ID = '%s'",
            node_id
        );
        if (r >= sizeof(node_id_restriction)) {
            g_log(
                BDM_LOG_DOMAIN,
                G_LOG_LEVEL_WARNING,
                "db_get_resource_datapoints(): node id restriction too long!"
            );
            return NULL;
        }
    }


    if ((resource_id == NULL) || (strcmp(resource_id, "*") == 0)) {
        resource_id_restriction[0] = '\0';
    } else {
        r = snprintf(
            resource_id_restriction,
            sizeof(resource_id_restriction),
            "AND Resources.Resource_ID = '%s'",
            resource_id
        );
        if (r >= sizeof(resource_id_restriction)) {
            g_log(
                BDM_LOG_DOMAIN,
                G_LOG_LEVEL_WARNING,
                "db_get_resource_datapoints(): resource id restriction too long!"
            );
            return NULL;
        }
    }


    datapoint_start_restriction[0] = '\0';
    if ( include_datapoints && 
        (datapoint_start != NULL) &&
	((datapoint_start->tv_sec != 0) || (datapoint_start->tv_usec != 0))) 
    {
	r = snprintf(
		datapoint_start_restriction, 
		sizeof(datapoint_start_restriction),
		"AND ("
		" Datapoints.Timestamp_Sec > %d"
		" OR (Datapoints.Timestamp_Sec = %d AND Datapoints.Timestamp_Usec >= %d)"
		")",
		(int)datapoint_start->tv_sec, 
		(int)datapoint_start->tv_sec, 
		(int)datapoint_start->tv_usec);
	if (r >= sizeof(datapoint_start_restriction)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "db_get_resource_datapoints(): datapoint start time is too long!");
	    return NULL;
	}
    }

    datapoint_end_restriction[0] = '\0';
    if ( include_datapoints && 
        (datapoint_end != NULL) &&
	((datapoint_end->tv_sec != 0) || (datapoint_end->tv_usec != 0))) 
    {
	r = snprintf(
		datapoint_end_restriction, 
		sizeof(datapoint_end_restriction),
		"AND ("
		" Datapoints.Timestamp_Sec < %d"
		" OR (Datapoints.Timestamp_Sec = %d AND Datapoints.Timestamp_Usec < %d)"
		")",
		(int)datapoint_end->tv_sec, (int)datapoint_end->tv_sec, 
		(int)datapoint_end->tv_usec);
	if (r >= sizeof(datapoint_end_restriction)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "db_get_resource_datapoints(): datapoint end time is too long!");
	    return NULL;
	}
    }

    if (include_datapoints) {
	if (entry_start < 0 && entry_end < 0) {
	    entry_restriction[0] = '\0';
	} else if ( entry_end < 0 ){
	    r = snprintf(entry_restriction, sizeof(entry_restriction),
			 "AND ("
			 "  Datapoints.Entry_Num >= %d"
			 ")",
			 entry_start);
	    if (r >= sizeof(entry_restriction)) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		      "db_get_resource_entrys(): entry start is too long!");
		return NULL;
	    }
	} else if ( entry_start < 0 ){
	    r = snprintf(entry_restriction, sizeof(entry_restriction),
			 "AND ("
			 "  Datapoints.Entry_Num <= %d"
			 ")",
			 entry_end);
	    if (r >= sizeof(entry_restriction)) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		      "db_get_resource_entrys(): entry end is too long!");
		return NULL;
	    }
	} else {
	    r = snprintf(entry_restriction, sizeof(entry_restriction),
			 "AND ("
			 "  (Datapoints.Entry_Num <= %d and Datapoints.Entry_Num >= %d )"
			 ")",
			 entry_end, entry_start);
	    if (r >= sizeof(entry_restriction)) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		      "db_get_resource_entrys(): entry end is too long!");
		return NULL;
	    }
	}
    } else {
	if (entry_start < 0 && entry_end < 0) {
	    entry_restriction[0] = '\0';
	} else if ( entry_end < 0 ){
	    r = snprintf(entry_restriction, sizeof(entry_restriction),
			 "AND ("
			 "  Hardware_Abstractors.Entry_Num >= %d OR"
			 "  Nodes.Entry_Num >= %d OR"
			 "  Resources.Entry_Num >= %d"
			 ")",
			 entry_start, entry_start, entry_start);
	    if (r >= sizeof(entry_restriction)) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		      "db_get_resource_entrys(): entry start is too long!");
		return NULL;
	    }
	} else if ( entry_start < 0 ){
	    r = snprintf(entry_restriction, sizeof(entry_restriction),
			 "AND ("
			 "  Hardware_Abstractors.Entry_Num <= %d OR"
			 "  Nodes.Entry_Num <= %d OR"
			 "  Resources.Entry_Num <= %d"
			 ")",
			 entry_end, entry_end, entry_end);
	    if (r >= sizeof(entry_restriction)) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		      "db_get_resource_entrys(): entry end is too long!");
		return NULL;
	    }
	} else {
	    r = snprintf(entry_restriction, sizeof(entry_restriction),
			 "AND ("
			 "  (Hardware_Abstractors.Entry_Num <= %d AND Hardware_Abstractors.Entry_Num >= %d ) OR"
			 "  (Nodes.Entry_Num <= %d AND Nodes.Entry_Num >= %d ) OR"
			 "  (Resources.Entry_Num <= %d AND Resources.Entry_Num >= %d )"
			 ")",
			 entry_end, entry_start, 
			 entry_end, entry_start, 
			 entry_end, entry_start);
	    if (r >= sizeof(entry_restriction)) {
		g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		      "db_get_resource_entrys(): entry end is too long!");
		return NULL;
	    }
	}
    }

    const char * sql_tables;
    const char * sql_fields;
    const char * sql_order = "";
    const char * sql_group = "";
    const char * sql_dp = "";
    if(include_datapoints){
        sql_fields = 
            " Hardware_Abstractors.HAB_TYPE,"
            " Hardware_Abstractors.HAB_ID,"
            " Nodes.Node_ID,"
            " Resource_Data_Types.Data_Type,"
            " Resource_Flavors.Flavor,"
            " Resources.Resource_ID,"
            " Datapoints.Value,"
            " Datapoints.Timestamp_Sec,"
            " Datapoints.Timestamp_Usec,"
            " BDMs.BDM_ID";

        sql_tables =
            "    Hardware_Abstractors,"
            "    Nodes,"
            "    Resources,"
            "    Resource_Data_Types,"
            "    Resource_Flavors,"
            "    Datapoints,"
            "    BDMs";

	sql_dp = 
	    "    AND Datapoints.Resource_Key=Resources.Key"
	    "    AND Datapoints.BDM_Key=BDMs.Key";
        sql_order =
            " ORDER BY"
            "     Datapoints.Timestamp_Sec ASC,"
            "     Datapoints.Timestamp_Usec ASC";
    } else {
        sql_fields = 
            " Hardware_Abstractors.HAB_TYPE,"
            " Hardware_Abstractors.HAB_ID,"
            " Nodes.Node_ID,"
            " Resource_Data_Types.Data_Type,"
            " Resource_Flavors.Flavor,"
            " Resources.Resource_ID,"
            " NULL,"
            " NULL,"
            " NULL,"
            " BDMs.BDM_ID";

        sql_tables =
            "    Hardware_Abstractors,"
            "    Nodes,"
            "    Resources,"
            "    Resource_Data_Types,"
            "    Resource_Flavors,"
            "    BDMs";

        sql_group = 
            " GROUP BY "
            " Hardware_Abstractors.HAB_TYPE,"
            " Hardware_Abstractors.HAB_ID,"
            " Nodes.Node_ID,"
            " Resource_Data_Types.Data_Type,"
            " Resource_Flavors.Flavor,"
            " Resources.Resource_ID";
    }

    r = snprintf(sql, sizeof(sql),
        "SELECT"
        "    %s"
        " FROM"
        "    %s"
        " WHERE"
        "    Nodes.HAB_Key=Hardware_Abstractors.Key"
        "    AND Resources.Node_Key=Nodes.Key"
        "    AND Resource_Data_Types.Key=Resources.Data_Type_Key"
        "    AND Resource_Flavors.Key=Resources.Flavor_Key"
        "    %s"
        "    %s"
        "    %s"
        "    %s"
        "    %s"
        "    %s"
        "    %s"
        "    %s"
        "    %s"
        " %s", //ORDER BY
        sql_fields,
        sql_tables,
        sql_dp,
        datapoint_start_restriction,
        datapoint_end_restriction,
        entry_restriction,
        hab_type_restriction,
        hab_id_restriction,
        node_id_restriction,
        resource_id_restriction,
        sql_group,
        sql_order
        );

    if (r >= sizeof(sql)) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "db_get_resource_datapoints(): SQL doesnt fit in buffer!\n");
        return NULL;
    }

    g_log(
        BDM_LOG_DOMAIN,
        G_LOG_LEVEL_DEBUG,
        "db_get_resource_datapoints(): SQL is %s",
        sql
    );

    sqlite3_stmt *stmt;
    r = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (r != SQLITE_OK) goto db_fail;

    for(;;) {
        r = sqlite3_step(stmt);
        if(r == SQLITE_BUSY) {
            g_usleep(20 * 1000);
            continue;
        }
        if(r == SQLITE_ROW) {
            _add_to_bdm_list(stmt, bdm_list);
            continue;
        }
        break;
    }
    if (r != SQLITE_DONE) goto db_fail;

    sqlite3_finalize(stmt);

    return bdm_list;

    db_fail:
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            "db_get_resource_datapoints(): SQL error: %s", sqlite3_errmsg(db));
        sqlite3_reset(stmt);
        sqlite3_finalize(stmt);
        g_ptr_array_free(bdm_list, TRUE);
        return NULL;

}

GPtrArray *db_get_resource_datapoints(
    sqlite3 *db,
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const char *resource_id,
    struct timeval *datapoint_start,
    struct timeval *datapoint_end,
    int entry_start,
    int entry_end
) {

    return _db_get_resource_info(
        db,
        hab_type,
        hab_id,
        node_id,
        resource_id,
        datapoint_start,
        datapoint_end,
        entry_start,
        entry_end,
        1);
}

GPtrArray *db_get_metadata(
    sqlite3 *db,
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const char *resource_id,
    struct timeval *datapoint_start,
    struct timeval *datapoint_end,
    int entry_start,
    int entry_end)
{

    return _db_get_resource_info(
        db,
        hab_type,
        hab_id,
        node_id,
        resource_id,
        datapoint_start,
        datapoint_end,
        entry_start,
        entry_end,
        0);
}

static int db_set_int_callback(
    void *sql_ret_void,
    int argc,
    char **argv,
    char **azColName)
{
    if (argc == 1 && argv[0]) {
        *(int*)sql_ret_void = atoi(argv[0]);
        return 0;
    }
    return -1;
}

static int db_get_next_entry_seq(sqlite3* db) {
    int r;
    char *zErrMsg = NULL;
    int latest_seq = -1;

    // start transaction
    r = do_begin_transaction(db);
    if (r != 0) return -1;

    // Delete last seq
    r = sqlite3_exec(
        db,
        "DELETE FROM Entry_Sequence",
        NULL,
        0,
        &zErrMsg
    );
    if (r != SQLITE_OK) goto fail;

    // Create New Seq
    r = sqlite3_exec(
        db,
        "INSERT INTO Entry_Sequence VALUES (NULL)",
        NULL,
        0,
        &zErrMsg
    );
    if (r != SQLITE_OK) goto fail;

    // Get the new value
    r = sqlite3_exec(
        db,
        "SELECT max(Num) from Entry_Sequence",
        db_set_int_callback,
        &latest_seq,
        &zErrMsg
    );
    if (r != SQLITE_OK) goto fail;

    // commit transaction
    r = do_commit(db);
    if (r != SQLITE_OK) goto fail;

    return latest_seq;

fail:
    if(zErrMsg){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Get Latest entry SQL error: %s\n", zErrMsg);
	sqlite3_free(zErrMsg);
    }
    r = sqlite3_exec(
        db,
        "ROLLBACK;",
        NULL,
        0,
        &zErrMsg
    );
    if (r != SQLITE_OK) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "ROLLBACK SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    return -1;
}

int db_get_latest_entry_seq(sqlite3 *db) {
    int seq = -1;
    int r;
    char * zErrMsg = NULL;

    r = sqlite3_exec(
        db,
        "SELECT max(Num) from ("
        "  SELECT max(Entry_num) as Num from Hardware_Abstractors"
        " UNION"
        "  SELECT max(Entry_num) as Num from Nodes"
        " UNION"
        "  SELECT max(Entry_num) as Num from Resources"
        " UNION"
        "  SELECT max(Entry_num) as Num from Datapoints"
        " UNION"
        "  SELECT 0 as Num"
        ")",
        db_set_int_callback,
        &seq,
        &zErrMsg
    );
    if (r != SQLITE_OK) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): SELECT SQL error: %s\n",
            __FUNCTION__, zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }
    return seq;
}


int db_get_last_sync_seq_metadata(sqlite3 *db, char * bdm_id) {
    int r;

    if(get_last_sync_bdm_stmt_metadata == NULL) {
	r = sqlite3_prepare_v2(db, 
	    "SELECT Last_Sync_Metadata"
            " FROM BDMs"
            " WHERE BDM_ID = ?",
	    -1, &get_last_sync_bdm_stmt_metadata, NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "get-last-sync SQL error: %s\n", sqlite3_errmsg(db));
	    return -1;
	}

    }

    return db_get_last_sync_seq(db, bdm_id, get_last_sync_bdm_stmt_metadata);
}
int db_get_last_sync_seq_datapoints(sqlite3 *db, char * bdm_id) {
    int r;

    if(get_last_sync_bdm_stmt_datapoints == NULL) {
	r = sqlite3_prepare_v2(db, 
	    "SELECT Last_Sync_Datapoints"
            " FROM BDMs"
            " WHERE BDM_ID = ?",
	    -1, &get_last_sync_bdm_stmt_datapoints, NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "get-last-sync SQL error: %s\n", sqlite3_errmsg(db));
	    return -1;
	}

    }

    return db_get_last_sync_seq(db, bdm_id, get_last_sync_bdm_stmt_datapoints);
}
static int db_get_last_sync_seq(sqlite3 *db, char * bdm_id, sqlite3_stmt *stmt) {
    int r;
    int seq = -1;

    r = sqlite3_bind_text(stmt, 1, bdm_id, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            "get-last-sync SQL bind error");
        return -1;
    }

    r = sqlite3_step(stmt);
    if (r != SQLITE_ROW && r != SQLITE_DONE) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "No row for last-sync: %s\n", sqlite3_errmsg(db));
	sqlite3_reset(stmt);
        return -1;
    }

    seq = sqlite3_column_int(stmt, 1);

    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);

    return seq;
}

void db_set_last_sync_seq_metadata(sqlite3 *db, char * bdm_id, int last_sync) {
    int r;

    if(set_last_sync_bdm_stmt_metadata == NULL) {
	r = sqlite3_prepare_v2(db, 
	    "UPDATE BDMs"
            " SET LAST_Sync_Metadata = ?"
            " WHERE BDM_ID = ?",
	    -1, &set_last_sync_bdm_stmt_metadata, NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "get-last-sync SQL error: %s\n", sqlite3_errmsg(db));
	    return;
	}

    }

    db_set_last_sync_seq(db, bdm_id, last_sync, set_last_sync_bdm_stmt_metadata);
}
void db_set_last_sync_seq_datapoints(sqlite3 *db, char * bdm_id, int last_sync) {
    int r;

    if(set_last_sync_bdm_stmt_datapoints == NULL) {
	r = sqlite3_prepare_v2(db, 
	    "UPDATE BDMs"
            " SET LAST_Sync_Datapoints = ?"
            " WHERE BDM_ID = ?",
	    -1, &set_last_sync_bdm_stmt_datapoints, NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "get-last-sync SQL error: %s\n", sqlite3_errmsg(db));
	    return;
	}

    }

    db_set_last_sync_seq(db, bdm_id, last_sync, set_last_sync_bdm_stmt_datapoints);
}

static void db_set_last_sync_seq(sqlite3 *db, char * bdm_id, int last_sync, sqlite3_stmt *stmt) {
    int r;

    // Bind variables
    r = sqlite3_bind_int(stmt, 1, last_sync);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            "set-last-sync SQL bind error");
        return;
    }
    r = sqlite3_bind_text(stmt, 2, bdm_id, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            "set-last-sync SQL bind error");
        return;
    }

    while(SQLITE_BUSY == (r = sqlite3_step(stmt))){
        g_usleep(20 * 1000);
    }
    if (r != SQLITE_DONE) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Error updating last-sync: %s\n", sqlite3_errmsg(db));
	sqlite3_reset(stmt);
        return;
    }

    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);

}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
