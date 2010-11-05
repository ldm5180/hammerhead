
// Copyright (c) 2008-2010, Regents of the University of Colorado.
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

#include <inttypes.h>

#include "bionet-data-manager.h"
#include "bdm-db.h"
#include "../../util/protected.h"

#include "bdm-stats.h"

#undef _DUMP_SQL
#undef _DEBUG_DB

extern const char * bdm_schema_sql;

int db_get_hab_rowid(
        sqlite3* db,
        const char * hab_type,
        const char * hab_id,
        sqlite_int64 *rowid);

int db_get_node_rowid(
        sqlite3* db,
        const char * node_id,
        const char * hab_type,
        const char * hab_id,
        uint8_t guid[BDM_RESOURCE_KEY_LENGTH],
        sqlite_int64 *rowid);

int db_get_bdm_rowid(
        sqlite3* db,
        const char *bdm_id,
        sqlite_int64 *rowid) ;

int db_get_datapoint_rowid(
        sqlite3* db,
        uint8_t resource_key[BDM_RESOURCE_KEY_LENGTH],
        bdm_datapoint_t * dp,
        sqlite_int64 *rowid);

int db_get_event_rowid(
        sqlite3* db,
        const struct timeval * timestamp,
        sqlite_int64 bdm_row,
        dbb_event_type_t event_type,
        sqlite_int64 data_row,
        sqlite_int64 *rowid);


sqlite3_stmt * all_stmts[NUM_PREPARED_STMTS] = {0};

extern char * database_file;


int datapoint_bionet_to_bdm(
    bionet_datapoint_t * datapoint,
    bdm_datapoint_t *dp)
{

    bionet_resource_data_type_t type;
    bionet_value_t *value;
    int r = -1;

    type = bionet_resource_get_data_type(bionet_datapoint_get_resource(datapoint));
    value = bionet_datapoint_get_value(datapoint);

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
            char * str = NULL;
            r = bionet_value_get_str(value, &str);
            if ( r == 0 ) {
                dp->value.str = strdup(str);
            } else {
                dp->value.str = NULL;
            }
            dp->type = DB_STRING;
            break;
        }
        default: {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s(): invalid datatype %d", 
                __FUNCTION__, type);
            return -1;
        }
    }
    return r;
}

bionet_datapoint_t * datapoint_bdm_to_bionet(
    bdm_datapoint_t *dp,
    bionet_resource_t * resource)
{
    bionet_resource_data_type_t type;
    bionet_value_t *value = NULL;
    bionet_datapoint_t *bionet_datapoint = NULL;

    type = bionet_resource_get_data_type(resource);

    struct timeval *ts = &dp->timestamp;

    // Set type and value
    switch (type) {
        case BIONET_RESOURCE_DATA_TYPE_BINARY:
            value = bionet_value_new_binary(resource, 
                dp->value.i);
            break; 

        case BIONET_RESOURCE_DATA_TYPE_UINT8:
            value = bionet_value_new_uint8(resource, 
                dp->value.i);
            break; 

        case BIONET_RESOURCE_DATA_TYPE_INT8:   
            value = bionet_value_new_int8(resource, 
                dp->value.i);
            break; 

        case BIONET_RESOURCE_DATA_TYPE_UINT16: 
            value = bionet_value_new_uint16(resource, 
                dp->value.i);
            break; 
            
        case BIONET_RESOURCE_DATA_TYPE_INT16:  
            value = bionet_value_new_int16(resource, 
                dp->value.i);
            break; 

        case BIONET_RESOURCE_DATA_TYPE_UINT32: 
            value = bionet_value_new_uint32(resource, 
                dp->value.i);
            break; 

        case BIONET_RESOURCE_DATA_TYPE_INT32:  
            value = bionet_value_new_int32(resource, 
                dp->value.i);
            break; 

        case BIONET_RESOURCE_DATA_TYPE_FLOAT:  
            value = bionet_value_new_float(resource, 
                dp->value.d);
            break; 

        case BIONET_RESOURCE_DATA_TYPE_DOUBLE: 
            value = bionet_value_new_double(resource, 
                dp->value.d);
            break; 

        case BIONET_RESOURCE_DATA_TYPE_STRING:
            value = bionet_value_new_str(resource, 
                dp->value.str);
            break; 

        case BIONET_RESOURCE_DATA_TYPE_INVALID:
        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
                  "%s(): invalid data_type %d for :%s", __FUNCTION__,
                  bionet_resource_get_data_type(resource),
                  bionet_resource_get_id(resource));
            break; 
    }

    if(value) {
        bionet_datapoint = bionet_datapoint_new(resource, value, ts);
    }

    return bionet_datapoint;
}



typedef struct sql_return {
    GPtrArray *hab_list;
} sql_return_t;

sqlite3 *db_init(const char * database_file) {
    int r;

    sqlite3 * db = NULL;

    // The version of sqlite3 on hardy doesn't provide this function
#if 0
    if (! sqlite3_threadsafe() ) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "ERROR: sqlite3 was not compiled threadsafe");
        return NULL;
    }
#endif


    r = sqlite3_open(database_file, &db);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error opening database %s: %s", database_file, sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    const char * dataType;
    const char * collSeq;
    int bNotNull;
    int bPrimaryKey;
    int bAutoInc;
    r = sqlite3_table_column_metadata(db, NULL, "Datapoints", "Key", 
            &dataType, &collSeq, &bNotNull, &bPrimaryKey, &bAutoInc);
    if (SQLITE_OK != r || !bPrimaryKey) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "No schema found in database '%s'. Creating one now...", database_file);

        char * errMsg = NULL;
        r = sqlite3_exec(db, bdm_schema_sql, NULL, NULL, &errMsg);
        if (r != SQLITE_OK) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                    "error creating schema: %s", errMsg);
            sqlite3_close(db);
            return NULL;
        }
    }


    r = sqlite3_busy_timeout(db, 5 * 1000);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error setting busy timeout on database: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    return db;
}


// 
// shut down the database
//

void db_shutdown(sqlite3 *db) {
    int i;
    for(i=0; i<NUM_PREPARED_STMTS; i++) {
        if(all_stmts[i]){
            sqlite3_finalize(all_stmts[i]);
            all_stmts[i] = NULL;
        }
    }

    sqlite3_close(db);
}




// this calls COMMIT and retries indefinately if the DB is busy
int db_commit(sqlite3 *db) {
    int r;
    char *zErrMsg = NULL;

    num_db_commits++;

    int attempts = 10;

    while((--attempts) > 0 ) {
        r = sqlite3_exec(
            db,
            "COMMIT;",
            NULL,
            0,
            &zErrMsg
        );

        if (r == SQLITE_OK) {
#ifdef _DEBUG_DB
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
                  "committed(%p)", db);
#endif
             return 0;
        }

        if (r == SQLITE_BUSY) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "COMMIT failed because the database is busy (\"%s\"), retrying %d more times", zErrMsg, attempts);
            sqlite3_free(zErrMsg);
            g_usleep(20 * 1000);
            continue;
        }

        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "COMMIT SQL error: %s", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    } 

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "COMMIT failed because the database is busy (\"%s\")", zErrMsg);
    return -1;

}

void db_rollback(sqlite3 *db) {
    int r;
    char *zErrMsg = NULL;

    r = sqlite3_exec(
        db,
        "ROLLBACK;",
        NULL,
        0,
        &zErrMsg
    );
    if (r != SQLITE_OK) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "ROLLBACK SQL error: %s", zErrMsg);
        sqlite3_free(zErrMsg);
    }
}

int db_begin_transaction(sqlite3 *db) {
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
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "BEGIN TRANSACTION SQL error: %s", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }

#ifdef _DEBUG_DB
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	  "in transaction(%p)", db);
#endif

    return 0;
}

// 
// database back-end internal helper functions
//
// Each of these functions does a single SQL INSERT.
// Returns 0 on success, -1 on failure.
//

int db_insert_hab(
        sqlite3* db,
        const char * hab_type,
        const char * hab_id,
        sqlite_int64 *rowid) 
{
    int r;


    if(all_stmts[INSERT_HAB_STMT] == NULL) {
	r = sqlite3_prepare_v2(db, 
            "INSERT"
            " INTO Hardware_Abstractors (HAB_Type, HAB_ID) "
            " VALUES (?,?)",
	    -1, &all_stmts[INSERT_HAB_STMT], NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "insert-hab SQL error: %s", sqlite3_errmsg(db));
	    return -1;
	}
    }
    sqlite3_stmt * this_stmt = all_stmts[INSERT_HAB_STMT];

    int param = 1;
    r = sqlite3_bind_text(this_stmt, param++, hab_type, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "insert-hab SQL bind error");
	return -1;
    }

    r = sqlite3_bind_text(this_stmt, param, hab_id, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "insert-hab SQL bind error");
	return -1;
    }

#ifdef _DEBUG_DB
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	  "%s(%p): %s.%s",
          __FUNCTION__, db,
          hab_type, hab_id);
#endif

    while(SQLITE_BUSY == (r = sqlite3_step(this_stmt))){
        g_usleep(20 * 1000);
    }
        
    int ret = 0;
    switch (r) {
        case SQLITE_DONE:
            // Success!
            if(rowid) {
                *rowid = sqlite3_last_insert_rowid(db);
            }
            break;

        case SQLITE_CONSTRAINT:
            // Row already exists. That's cool...
            if(rowid) {
                ret = db_get_hab_rowid(db, hab_type, hab_id, rowid);
                if(0 == ret) {
                    ret = 1; // Row already existed
                }
            }
            break;

        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "insert-hab SQL error: %s", sqlite3_errmsg(db));
            ret = -1;
    }

    sqlite3_reset(this_stmt);
    sqlite3_clear_bindings(this_stmt);


    return ret;
}

int db_get_hab_rowid(
        sqlite3* db,
        const char * hab_type,
        const char * hab_id,
        sqlite_int64 *rowid) 
{
    int ret = 0;
    int r;

    if(all_stmts[ROWFOR_HAB_STMT] == NULL) {
	r = sqlite3_prepare_v2(db, 
            "SELECT Key "
            "FROM Hardware_Abstractors "
            "WHERE Hab_Type = ? "
            " AND   Hab_Id = ? ",
	    -1, &all_stmts[ROWFOR_HAB_STMT], NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "insert-hab SQL error: %s", sqlite3_errmsg(db));
	    return -1;
	}
    }
    sqlite3_stmt * this_stmt = all_stmts[ROWFOR_HAB_STMT];

    int param = 1;
    r = sqlite3_bind_text(this_stmt, param++, hab_type, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "insert-hab SQL bind error");
	return -1;
    }

    r = sqlite3_bind_text(this_stmt, param, hab_id, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "insert-hab SQL bind error");
	return -1;
    }

    while(SQLITE_BUSY == (r = sqlite3_step(this_stmt))){
        g_usleep(20 * 1000);
    }
        
    switch (r) {
        case SQLITE_DONE:
        case SQLITE_ROW:
            // Success!
            if(rowid) {
                *rowid = sqlite3_column_int64(this_stmt, 0);
            }
            break;

        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "row-for-hab SQL error: %s", sqlite3_errmsg(db));
            ret = -1;
    }

    sqlite3_reset(this_stmt);
    sqlite3_clear_bindings(this_stmt);

    return ret;
}


int db_insert_node(
        sqlite3* db,
        const char * node_id,
        const char * hab_type,
        const char * hab_id,
        uint8_t guid[BDM_RESOURCE_KEY_LENGTH],
        sqlite_int64 *rowid)
{
    int r;

    if(all_stmts[INSERT_NODE_STMT] == NULL) {
	r = sqlite3_prepare_v2(db, 
            "INSERT"
            " INTO Nodes (GUID, HAB_Key, Node_ID)"
            " SELECT"
            "     ?, Hardware_Abstractors.Key, ?"
            "     FROM Hardware_Abstractors"
            "     WHERE"
            "         HAB_Type=?"
            "         AND HAB_ID=?",
            -1, &all_stmts[INSERT_NODE_STMT], NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "insert-node SQL error: %s", sqlite3_errmsg(db));
	    return -1;
	}
    }
    sqlite3_stmt * this_stmt = all_stmts[INSERT_NODE_STMT];


    int param = 1;
    r = sqlite3_bind_blob(this_stmt, param++, 
        guid, BDM_RESOURCE_KEY_LENGTH, SQLITE_TRANSIENT);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "insert-node SQL bind error: %d", param);
	return -1;
    }

    r = sqlite3_bind_text(this_stmt, param++, node_id, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "insert-node SQL bind error");
	return -1;
    }

    r = sqlite3_bind_text(this_stmt, param++, hab_type, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "insert-node SQL bind error");
	return -1;
    }

    r = sqlite3_bind_text(this_stmt, param, hab_id, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "insert-node SQL bind error");
	return -1;
    }

#ifdef _DEBUG_DB
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	  "db_insert_node(%p): %s.%s.%s [" UUID_FMTSTR "]",
          db,
          hab_type, hab_id, node_id, UUID_ARGS(guid));
#endif

    while(SQLITE_BUSY == (r = sqlite3_step(this_stmt))){
        g_usleep(20 * 1000);
    }

        
    int ret = 0;
    switch (r) {
        case SQLITE_DONE:
            // Success!
            if(rowid) {
                *rowid = sqlite3_last_insert_rowid(db);
            }
            break;

        case SQLITE_CONSTRAINT:
            // Row already exists. That's cool...
            if(rowid) {
                ret = db_get_node_rowid(db, node_id, hab_type, hab_id, guid, rowid);
                if(0 == ret) {
                    ret = 1; // Row already existed
                }
#ifdef _DEBUG_DB
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
                      "%s(%p): => %" PRId64,
                      __FUNCTION__, db, (int64_t)*rowid);
#endif
            }
            break;

        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "insert-node SQL error: %s", sqlite3_errmsg(db));
            ret = -1;
    }

    sqlite3_reset(this_stmt);
    sqlite3_clear_bindings(this_stmt);

    return ret;
}

int db_get_node_rowid(
        sqlite3* db,
        const char * node_id,
        const char * hab_type,
        const char * hab_id,
        uint8_t guid[BDM_RESOURCE_KEY_LENGTH],
        sqlite_int64 *rowid)
{
    int r;

    if(all_stmts[ROWFOR_NODE_STMT] == NULL) {
	r = sqlite3_prepare_v2(db, 
            "SELECT Key"
            " FROM Nodes "
            " WHERE GUID = ? "
            "   AND Node_ID = ? ",
            -1, &all_stmts[ROWFOR_NODE_STMT], NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "insert-node SQL error: %s", sqlite3_errmsg(db));
	    return -1;
	}
    }
    sqlite3_stmt * this_stmt = all_stmts[ROWFOR_NODE_STMT];


    int param = 1;
    r = sqlite3_bind_blob(this_stmt, param++, 
        guid, BDM_RESOURCE_KEY_LENGTH, SQLITE_TRANSIENT);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "insert-node SQL bind error: %d", param);
	return -1;
    }

    r = sqlite3_bind_text(this_stmt, param, node_id, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "insert-node SQL bind error");
	return -1;
    }


    while(SQLITE_BUSY == (r = sqlite3_step(this_stmt))){
        g_usleep(20 * 1000);
    }

        
    int ret = 0;
    switch (r) {
        case SQLITE_DONE:
        case SQLITE_ROW:
            // Success!
            if(rowid) {
                *rowid = sqlite3_column_int64(this_stmt, 0);
            }
            break;

        case SQLITE_CONSTRAINT:
            // Row already exists. That's cool... TODO: Is it?
            break;

        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "insert-node SQL error: %s", sqlite3_errmsg(db));
            ret = -1;
    }

    sqlite3_reset(this_stmt);
    sqlite3_clear_bindings(this_stmt);

    return ret;
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
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error initializing SHA1 context");
        return -1;
    }

    r = SHA1_Update(&sha_ctx, hab_type, strlen(hab_type));
    if (r != 1) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource HAB-Type");
	return -1;
    }
    
    r = SHA1_Update(&sha_ctx, hab_id, strlen(hab_id));
    if (r != 1) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource HAB-ID");
	return -1;
    }

    r = SHA1_Update(&sha_ctx, node_id, strlen(node_id));
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource Node-ID");
        return -1;
    }

    r = SHA1_Update(&sha_ctx, resource_id, strlen(resource_id));
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource ID");
        return -1;
    }

    byte = data_type;
    r = SHA1_Update(&sha_ctx, &byte, 1);
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource Data Type");
        return -1;
    }

    byte = flavor;
    r = SHA1_Update(&sha_ctx, &byte, 1);
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource Flavor");
        return -1;
    }

    r = SHA1_Final(sha_digest, &sha_ctx);
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error finalizing SHA1 digest");
        return -1;
    }

    memcpy(resource_key, sha_digest,BDM_RESOURCE_KEY_LENGTH);

    return 0;

}


int db_insert_resource(
        sqlite3* db,
        const char * hab_type,
        const char * hab_id,
        const char * node_id,
        uint8_t node_guid[BDM_UUID_LEN],
        const char * resource_id,
        bionet_resource_flavor_t flavor,
        bionet_resource_data_type_t data_type)
{
    int r;

    uint8_t resource_key[BDM_RESOURCE_KEY_LENGTH];

    r = db_make_resource_key(hab_type, hab_id, node_id, 
        resource_id, data_type, flavor, resource_key);
    if(r != 0){
        return r;
    }

#ifdef _DUMP_SQL
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
            "INSERT"
            " INTO Resources"
            " SELECT"
            "     '%s', Nodes.Key, '%s', Resource_Data_Types.Key, Resource_Flavors.Key"
            "     FROM Hardware_Abstractors, Nodes, Resource_Data_Types, Resource_Flavors"
            "     WHERE"
            "         Hardware_Abstractors.HAB_Type='%s'"
            "         AND Hardware_Abstractors.HAB_ID='%s'"
            "         AND Nodes.HAB_Key=Hardware_Abstractors.Key"
            "         AND Nodes.Node_ID='%s'"
            "         AND Nodes.GUID=?"
            "         AND Resource_Data_Types.Data_Type LIKE '%s'"
            "         AND Resource_Flavors.Flavor LIKE '%s'",
                "fingerprint", resource_id, hab_type, hab_id, node_id,   bionet_resource_data_type_to_string(data_type),   bionet_resource_flavor_to_string(flavor));

#endif

    if(all_stmts[INSERT_RESOURCE_STMT] == NULL) {
	r = sqlite3_prepare_v2(db, 
            "INSERT"
            " INTO Resources"
            " SELECT"
            "     ?, Nodes.Key, ?, Resource_Data_Types.Key, Resource_Flavors.Key"
            "     FROM Hardware_Abstractors, Nodes, Resource_Data_Types, Resource_Flavors"
            "     WHERE"
            "         Hardware_Abstractors.HAB_Type=?"
            "         AND Hardware_Abstractors.HAB_ID=?"
            "         AND Nodes.HAB_Key=Hardware_Abstractors.Key"
            "         AND Nodes.Node_ID=?"
            "         AND Nodes.GUID=?"
            "         AND Resource_Data_Types.Data_Type LIKE ?"
            "         AND Resource_Flavors.Flavor LIKE ?",
	    -1, &all_stmts[INSERT_RESOURCE_STMT], NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL error: %s", sqlite3_errmsg(db));
	    return -1;
	}
    }
    sqlite3_stmt * this_stmt = all_stmts[INSERT_RESOURCE_STMT];

    int param = 1;
    r = sqlite3_bind_blob(this_stmt, param++, resource_key, BDM_RESOURCE_KEY_LENGTH, SQLITE_TRANSIENT);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL bind error: %d", param);
	return -1;
    }
    r = sqlite3_bind_text(this_stmt, param++, resource_id, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL bind error: %d", param);
	return -1;
    }
    r = sqlite3_bind_text(this_stmt, param++, hab_type, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL bind error: %d", param);
	return -1;
    }
    r = sqlite3_bind_text(this_stmt, param++, hab_id, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL bind error: %d", param);
	return -1;
    }
    r = sqlite3_bind_text(this_stmt, param++, node_id, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL bind error: %d", param);
	return -1;
    }
    r = sqlite3_bind_blob(this_stmt, param++, 
        node_guid, BDM_RESOURCE_KEY_LENGTH, SQLITE_TRANSIENT);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL bind error: %d", param);
	return -1;
    }
    r = sqlite3_bind_text(this_stmt, param++, bionet_resource_data_type_to_string(data_type), -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL bind error: %d", param);
	return -1;
    }
    r = sqlite3_bind_text(this_stmt, param, bionet_resource_flavor_to_string(flavor), -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL bind error: %d", param);
	return -1;
    }

#ifdef _DEBUG_DB
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	  "%s(%p): %s.%s.%s:%s: " UUID_FMTSTR, 
          __FUNCTION__, db,
          hab_type, hab_id, node_id, resource_id,
          UUID_ARGS(resource_key));
#endif


    while(SQLITE_BUSY == (r = sqlite3_step(this_stmt))){
        g_usleep(20 * 1000);
    }
        
    int ret = 0;
    switch (r) {
        case SQLITE_DONE:
            if(1 == sqlite3_changes(db)) {
                // Success!
            } else {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                        "%s(): No rows modified (%d). %s.%s.%s:%s (%s,%s)", 
                        __FUNCTION__, sqlite3_changes(db),
                        hab_type, hab_id, node_id, resource_id,
                        bionet_resource_data_type_to_string(data_type),
                        bionet_resource_flavor_to_string(flavor));
                ret = -1;
            }
            break;

        case SQLITE_CONSTRAINT:
            // Row already exists. That's cool...
            ret = 1;
            break;

        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                    "add-resource SQL error: %s", sqlite3_errmsg(db));
            ret = -1;
    }

    sqlite3_reset(this_stmt);
    sqlite3_clear_bindings(this_stmt);


    return ret;

}


int db_insert_bdm(
        sqlite3* db,
        const char *bdm_id,
        sqlite_int64 *rowid) 
{
    int r;

    if(all_stmts[INSERT_BDM_STMT] == NULL) {
	r = sqlite3_prepare_v2(db, 
	    "INSERT"
	    " INTO BDMs(BDM_ID) VALUES(?)",
	    -1, &all_stmts[INSERT_BDM_STMT], NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-bdm SQL error: %s", sqlite3_errmsg(db));
	    return -1;
	}
    }
    sqlite3_stmt * this_stmt = all_stmts[INSERT_BDM_STMT];

    int param = 1;
    r = sqlite3_bind_text(this_stmt, param, bdm_id, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-bdm SQL bind error");
	return -1;
    }

    while(SQLITE_BUSY == (r = sqlite3_step(this_stmt))){
        g_usleep(20 * 1000);
    }

    int ret = 0;
    switch (r) {
        case SQLITE_DONE:
            // Success!
            if(rowid) {
                *rowid = sqlite3_last_insert_rowid(db);
            }
            break;

        case SQLITE_CONSTRAINT:
            // Row already exists. That's cool...
            if(rowid) {
                ret = db_get_bdm_rowid(db, bdm_id, rowid);
                if(0 == ret) {
                    ret = 1; // Row already existed
                }
            }
            break;

        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-bdm SQL error: %s", sqlite3_errmsg(db));
            ret = -1;
    }

    sqlite3_reset(this_stmt);
    sqlite3_clear_bindings(this_stmt);


    return ret;

}

int db_get_bdm_rowid(
        sqlite3* db,
        const char *bdm_id,
        sqlite_int64 *rowid) 
{
    int r;

    if(all_stmts[ROWFOR_BDM_STMT] == NULL) {
	r = sqlite3_prepare_v2(db, 
	    "SELECT Key "
	    "  FROM BDMs "
            " WHERE BDM_ID = ?",
	    -1, &all_stmts[ROWFOR_BDM_STMT], NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-bdm SQL error: %s", sqlite3_errmsg(db));
	    return -1;
	}
    }
    sqlite3_stmt * this_stmt = all_stmts[ROWFOR_BDM_STMT];

    int param = 1;
    r = sqlite3_bind_text(this_stmt, param, bdm_id, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-bdm SQL bind error");
	return -1;
    }

    while(SQLITE_BUSY == (r = sqlite3_step(this_stmt))){
        g_usleep(20 * 1000);
    }

    int ret = 0;
    switch (r) {
        case SQLITE_DONE:
        case SQLITE_ROW:
            // Success!
            if(rowid) {
                *rowid = sqlite3_column_int64(this_stmt, 0);
            }
            break;

        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                    "add-bdm SQL error: %s", sqlite3_errmsg(db));
            ret = -1;
    }

    sqlite3_reset(this_stmt);
    sqlite3_clear_bindings(this_stmt);


    return ret;

}

static int bind_dp_value(sqlite3_stmt* this_stmt, int param, bdm_datapoint_t *dp)
{
    int r;
    switch(dp->type) {
        case DB_INT:
            r = sqlite3_bind_int(this_stmt, param, dp->value.i);
#ifdef _DEBUG_DB
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
		  "%s(): (unsigned int)%lu (int)%li", __FUNCTION__, 
                  (unsigned long)dp->value.i, (long)dp->value.i);
#endif
            break;
            
        case DB_DOUBLE:
	    r = sqlite3_bind_double(this_stmt, param, dp->value.d);
#ifdef _DEBUG_DB
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
		  "%s(): (double)%.16g", __FUNCTION__, dp->value.d);
#endif
            break;
            
        case DB_STRING:
            r = sqlite3_bind_text(this_stmt, param, dp->value.str, -1, SQLITE_STATIC);
#ifdef _DEBUG_DB
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
		  "%s(): (str)%s", __FUNCTION__, dp->value.str);
#endif
            break;

        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add_datapoint_to_db internal error: type %d invalid", dp->type);
            return -1;

    }
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add_datapoint_to_db SQL bind error: %d", param);
	return -1;
    }
    return 0;
}

/*
 * Add the datapoint to the DB. 
 * Does NOT require metadata be inserted first
 */
int db_insert_datapoint(sqlite3* db, 
    uint8_t resource_key[BDM_RESOURCE_KEY_LENGTH],
    bdm_datapoint_t *dp,
    sqlite_int64 *rowid)
{
    int r;

    if(all_stmts[INSERT_DATAPOINT_SYNC_STMT] == NULL) {
	r = sqlite3_prepare_v2(db, 
	    "INSERT"
	    " INTO Datapoints"
	    " VALUES (NULL, ?, ?, ?, ?)",
	    -1, &all_stmts[INSERT_DATAPOINT_SYNC_STMT], NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add_datapoint_to_db SQL error: %s", sqlite3_errmsg(db));
	    return -1;
	}
    }
    sqlite3_stmt * this_stmt = all_stmts[INSERT_DATAPOINT_SYNC_STMT];

    // Bind host variables to the prepared statement 
    // -- This eliminates the need to escape strings
    // Bind in order of the placeholders (?) in the SQL
    int param = 1;
    r = sqlite3_bind_blob(this_stmt, param++, 
        resource_key, BDM_RESOURCE_KEY_LENGTH, SQLITE_TRANSIENT);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add_datapoint_to_db SQL bind error: %d", param);
	return -1;
    }

    r = bind_dp_value(this_stmt, param++, dp);
    if(r != 0) return -1;

    r = sqlite3_bind_int( this_stmt, param++,  dp->timestamp.tv_sec);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add_datapoint_to_db SQL bind error: %d", param);
	return -1;
    }

    r = sqlite3_bind_int( this_stmt, param,  dp->timestamp.tv_usec);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add_datapoint_to_db SQL bind error: %d", param);
	return -1;
    }

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	  "add_datapoint_to_db(%p): " UUID_FMTSTR,
          db, UUID_ARGS(resource_key));


    while(SQLITE_BUSY == (r = sqlite3_step(this_stmt))){
        g_usleep(20 * 1000);
    }

    int ret = 0;
    switch (r) {
        case SQLITE_DONE:
            // Success!
            if(rowid) {
                *rowid = sqlite3_last_insert_rowid(db);
            }
            break;

        case SQLITE_CONSTRAINT:
            // Row already exists. That's cool...
            if(rowid) {
                ret = db_get_datapoint_rowid(db, resource_key, dp, rowid);
                if(0 == ret) {
                    ret = 1; // Row already existed
                }
            }
            break;

        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add_datapoint_to_db SQL error: %s", sqlite3_errmsg(db));
            ret = -1;
    }

    if(rowid) {
#ifdef _DEBUG_DB
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
              "add_datapoint_to_db(%p): => %" PRId64,
          db, (int64_t)*rowid);
#endif
    }


    sqlite3_reset(this_stmt);
    sqlite3_clear_bindings(this_stmt);


    return ret;

}

int db_get_datapoint_rowid(sqlite3* db, 
    uint8_t resource_key[BDM_RESOURCE_KEY_LENGTH],
    bdm_datapoint_t *dp,
    sqlite_int64 *rowid)
{
    int r;

    if(all_stmts[ROWFOR_DATAPOINT_STMT] == NULL) {
	r = sqlite3_prepare_v2(db, 
	    "SELECT Key"
	    " FROM Datapoints"
	    " WHERE Resource_key = ?"
            "   AND Value = ?"
            "   AND Timestamp_Sec = ?"
            "   AND Timestamp_Usec = ?",
	    -1, &all_stmts[ROWFOR_DATAPOINT_STMT], NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s SQL error: %s", 
                    __FUNCTION__, sqlite3_errmsg(db));
	    return -1;
	}
    }
    sqlite3_stmt * this_stmt = all_stmts[ROWFOR_DATAPOINT_STMT];

    // Bind host variables to the prepared statement 
    // -- This eliminates the need to escape strings
    // Bind in order of the placeholders (?) in the SQL
    int param = 1;
    r = sqlite3_bind_blob(this_stmt, param++, 
        resource_key, BDM_RESOURCE_KEY_LENGTH, SQLITE_TRANSIENT);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s SQL bind error: %d", 
                __FUNCTION__, param);
	return -1;
    }

    r = bind_dp_value(this_stmt, param++, dp);
    if(r != 0) return -1;

    r = sqlite3_bind_int( this_stmt, param++,  dp->timestamp.tv_sec);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s SQL bind error: %d", 
                __FUNCTION__, param);
	return -1;
    }

    r = sqlite3_bind_int( this_stmt, param,  dp->timestamp.tv_usec);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s SQL bind error: %d", 
                __FUNCTION__, param);
	return -1;
    }

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
	  "%s(%p): " UUID_FMTSTR,
          __FUNCTION__,
          db, UUID_ARGS(resource_key));


    while(SQLITE_BUSY == (r = sqlite3_step(this_stmt))){
        g_usleep(20 * 1000);
    }

    int ret = 0;
    switch (r) {
        case SQLITE_DONE:
        case SQLITE_ROW:
            // Success!
            if(rowid) {
                *rowid = sqlite3_column_int64(this_stmt, 0);
            }
            break;

        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s SQL error: %s", 
                    __FUNCTION__, sqlite3_errmsg(db));
            ret = -1;
    }

    sqlite3_reset(this_stmt);
    sqlite3_clear_bindings(this_stmt);


    return ret;

}

static int bind_event_params(
        sqlite3_stmt* this_stmt,
        const struct timeval * timestamp,
        sqlite_int64 bdm_row,
        dbb_event_type_t event_type,
        sqlite_int64 data_row)
{
    // Bind host variables to the prepared statement 
    // -- This eliminates the need to escape strings
    // Bind in order of the placeholders (?) in the SQL
    int param = 1;
    int r;
    r = sqlite3_bind_int(this_stmt, param++, timestamp->tv_sec);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s SQL bind error: %d", 
                __FUNCTION__, param);
	return -1;
    }

    r = sqlite3_bind_int(this_stmt, param++, timestamp->tv_usec);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s SQL bind error: %d", 
                __FUNCTION__, param);
	return -1;
    }

    r = sqlite3_bind_int64(this_stmt, param++, bdm_row);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s SQL bind error: %d", 
                __FUNCTION__, param);
	return -1;
    }

    int islost=0;
    int idx = 0;
    switch(event_type) {
        case DBB_NEW_HAB_EVENT:
            idx=0;
            islost=0;
            break;

        case DBB_NEW_NODE_EVENT:
            idx=1;
            islost=0;
            break;

        case DBB_DATAPOINT_EVENT:
            idx=2;
            islost=0;
            break;

        case DBB_LOST_HAB_EVENT:
            idx=0;
            islost=1;
            break;

        case DBB_LOST_NODE_EVENT:
            idx=1;
            islost=1;
            break;
    }

    r = sqlite3_bind_int(this_stmt, param++, islost);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s SQL bind error: %d", 
                __FUNCTION__, param);
        return -1;
    }

    // Bind all params to NULL, except idx
    int i;
    for(i=0; i<3; i++) {
        if(i == idx) {
            r = sqlite3_bind_int64(this_stmt, param++, data_row);
        } else {
            r = sqlite3_bind_int64(this_stmt, param++, 0);
        }
        if(r != SQLITE_OK){
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s SQL bind error: %d", 
                    __FUNCTION__, param);
            return -1;
        }
    }

    return 0;
}

int db_get_event_rowid(
        sqlite3* db,
        const struct timeval * timestamp,
        sqlite_int64 bdm_row,
        dbb_event_type_t event_type,
        sqlite_int64 data_row,
        sqlite_int64 *rowid)
{
    int r;

    if(all_stmts[ROWFOR_EVENT_STMT] == NULL) {
	r = sqlite3_prepare_v2(db, 
	    "SELECT seq"
	    " FROM Events"
	    " WHERE timestamp_sec  = ?"
            "   AND timestamp_usec = ?"
            "   AND recording_bdm  = ?"
            "   AND islost         = ?"
            "   AND hab            = ?"
            "   AND node           = ?"
            "   AND datapoint      = ?",
	    -1, &all_stmts[ROWFOR_EVENT_STMT], NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s SQL error: %s", 
                    __FUNCTION__, sqlite3_errmsg(db));
	    return -1;
	}
    }
    sqlite3_stmt * this_stmt = all_stmts[ROWFOR_EVENT_STMT];

    r = bind_event_params(this_stmt, timestamp, bdm_row, event_type, data_row);
    if ( r != 0 ) return r;


    while(SQLITE_BUSY == (r = sqlite3_step(this_stmt))){
        g_usleep(20 * 1000);
    }

    int ret = 0;
    switch (r) {
        case SQLITE_DONE:
        case SQLITE_ROW:
            // Success!
            if(rowid) {
                *rowid = sqlite3_column_int64(this_stmt, 0);
            }
            break;

        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s SQL error: %s", 
                    __FUNCTION__, sqlite3_errmsg(db));
            ret = -1;
    }

    sqlite3_reset(this_stmt);
    sqlite3_clear_bindings(this_stmt);


    return ret;

}

int db_insert_event(
    sqlite3 * db,
    const struct timeval * timestamp,
    sqlite_int64 bdm_row,
    dbb_event_type_t event_type,
    sqlite_int64 data_row,
    sqlite_int64 *rowid)
{
    int r;
    struct timeval ts_dat;

    if(data_row < 0) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "%s() Bad data row passed in: %lld", 
                __FUNCTION__, data_row);

    }

    if(timestamp == NULL) {
        r = gettimeofday(&ts_dat, NULL);
        if ( r != 0 ) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): gettimeofday() returned error: %s", 
                    __FUNCTION__, strerror(errno));
            return -1;

        }
        timestamp = &ts_dat;
    }

    if(all_stmts[INSERT_EVENT_STMT] == NULL) {
	r = sqlite3_prepare_v2(db, 
	    "INSERT"
	    " INTO Events (timestamp_sec,timestamp_usec,recording_bdm, islost, hab, node, datapoint)"
	    " VALUES (?, ?, ?, ?, ?, ?, ?)",
	    -1, &all_stmts[INSERT_EVENT_STMT], NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                    "insert_event SQL error: %s", sqlite3_errmsg(db));
	    return -1;
	}
    }
    sqlite3_stmt * this_stmt = all_stmts[INSERT_EVENT_STMT];

    r = bind_event_params(this_stmt, timestamp, bdm_row, event_type, data_row);
    if ( r != 0 ) return r;

    while(SQLITE_BUSY == (r = sqlite3_step(this_stmt))){
        g_usleep(20 * 1000);
    }

    int ret = 0;
    switch (r) {
        case SQLITE_DONE:
            // Success!
            if(rowid) {
                *rowid = sqlite3_last_insert_rowid(db);
#ifdef _DEBUG_DB
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
                      "%s(): => %" PRId64,
                      __FUNCTION__, (int64_t)*rowid);
#endif
            }
            break;

        case SQLITE_CONSTRAINT:
            // Row already exists. That's cool...
            if(rowid) {
                ret = db_get_event_rowid(db, timestamp, bdm_row, event_type, data_row, rowid);
                if(0 == ret) {
                    ret = 1; // Row already existed
                }
            }
            break;

        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s() SQL error: %s", 
                    __FUNCTION__, sqlite3_errmsg(db));
            ret = -1;
    }

    sqlite3_reset(this_stmt);
    sqlite3_clear_bindings(this_stmt);


    return ret;


}

static void hab_list_free_elements(GPtrArray *hab_list) {
    int h;
    for (h = 0; h < hab_list->len; h++) {
        bionet_hab_t * hab = g_ptr_array_index(hab_list, h);
        bionet_hab_free(hab);
    }
}

void bdm_list_free(GPtrArray *bdm_list) {
    int b;
    for (b = 0; b < bdm_list->len; b++) {
        bionet_bdm_t * bdm = g_ptr_array_index(bdm_list, b);

        GPtrArray * hab_list = bionet_bdm_get_hab_list(bdm);
        hab_list_free_elements(hab_list);

        bionet_bdm_free(bdm);
    }

    g_ptr_array_free(bdm_list, TRUE);
}

void hab_list_free(GPtrArray *hab_list) {
    hab_list_free_elements(hab_list);

    g_ptr_array_free(hab_list, TRUE);
}

static bionet_bdm_t *find_bdm(GPtrArray * bdm_list, const char *bdm_id) {
    int i;
    bionet_bdm_t *bdm;

    for (i = 0; i < bdm_list->len; i ++) {
        bdm = g_ptr_array_index(bdm_list, i);
	if ( strcmp(bionet_bdm_get_id(bdm), bdm_id) == 0 )
        {
            return bdm;
        }
    }

    //
    // the requested bdm is not in the list, so add it
    //

    bdm = bionet_bdm_new(bdm_id);
    if (bdm == NULL) return NULL;

    g_ptr_array_add(bdm_list, bdm);
    return bdm;
}

static bionet_hab_t *find_hab(GPtrArray * hab_list, const char *hab_type, const char *hab_id) {
    int i;
    bionet_hab_t *hab;

    for (i = 0; i < hab_list->len; i ++) {
        hab = g_ptr_array_index(hab_list, i);
	if ( strcmp(bionet_hab_get_id(hab), hab_id) == 0
        &&   strcmp(bionet_hab_get_type(hab), hab_type) == 0) 
        {
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

static bionet_node_t *find_node(bionet_hab_t *hab, const char *node_id, const uint8_t node_uid[BDM_UUID_LEN]) {
    bionet_node_t *node;

    node = bionet_hab_get_node_by_id_and_uid(hab, node_id, node_uid);
    if (node != NULL) return node;

    node = bionet_node_new(hab, node_id);
    if (node == NULL) {
        return NULL;
    }

    bionet_node_set_uid(node, node_uid);

    if (bionet_hab_add_node(hab, node)) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "find_node(): Failed to add node to hab.");
	bionet_node_free(node);
	return NULL;
    }

    return node;
}


static bionet_resource_t *find_resource(
        bionet_node_t *node,
        bionet_resource_data_type_t data_type,
        bionet_resource_flavor_t flavor,
        const char *resource_id)
{
    bionet_resource_t *resource;

    resource = bionet_node_get_resource_by_id(node, resource_id);
    if (resource != NULL) return resource;

    resource = bionet_resource_new(node, data_type, flavor, resource_id);
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


// This is a specialized function to extract a manifest-typed value from the
// sqlite3 query result into a bionet resource value
static bionet_value_t * _sql_value_to_bionet(
        sqlite3_stmt * stmt,
        int column_idx,
        bionet_resource_t * resource)
{
    bionet_value_t * value = NULL;
    int column_type = sqlite3_column_type(stmt, column_idx);
    if(column_type != SQLITE_NULL){
        switch(bionet_resource_get_data_type(resource))
        {
        case BIONET_RESOURCE_DATA_TYPE_BINARY:
            value = bionet_value_new_binary(resource, 
                sqlite3_column_int(stmt, column_idx));
            break; 

        case BIONET_RESOURCE_DATA_TYPE_UINT8:
            value = bionet_value_new_uint8(resource, 
                (uint8_t)sqlite3_column_int(stmt, column_idx));
            break; 

        case BIONET_RESOURCE_DATA_TYPE_INT8:   
            value = bionet_value_new_int8(resource, 
                (int8_t)sqlite3_column_int(stmt, column_idx));
            break; 

        case BIONET_RESOURCE_DATA_TYPE_UINT16: 
            value = bionet_value_new_uint16(resource, 
                (uint16_t)sqlite3_column_int(stmt, column_idx));
            break; 
            
        case BIONET_RESOURCE_DATA_TYPE_INT16:  
            value = bionet_value_new_int16(resource, 
                (int16_t)sqlite3_column_int(stmt, column_idx));
            break; 

        case BIONET_RESOURCE_DATA_TYPE_UINT32: 
            value = bionet_value_new_uint32(resource, 
                (uint32_t)sqlite3_column_int(stmt, column_idx));
            break; 

        case BIONET_RESOURCE_DATA_TYPE_INT32:  
            value = bionet_value_new_int32(resource, 
                (int32_t)sqlite3_column_int(stmt, column_idx));
            break; 

        case BIONET_RESOURCE_DATA_TYPE_FLOAT:  
            value = bionet_value_new_float(resource, 
                (float)sqlite3_column_double(stmt, column_idx));
            break; 

        case BIONET_RESOURCE_DATA_TYPE_DOUBLE: 
            value = bionet_value_new_double(resource, 
                sqlite3_column_double(stmt, column_idx));
            break; 

        case BIONET_RESOURCE_DATA_TYPE_STRING:
            value = bionet_value_new_str(resource, 
                (const char*)sqlite3_column_text(stmt, column_idx));
            break; 

        case BIONET_RESOURCE_DATA_TYPE_INVALID:
        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
                  "db_get_resource_datapoints_callback(): invalid data_type %d for %s",
                  bionet_resource_get_data_type(resource),
                  bionet_resource_get_id(resource));
            break; 
        }
    } else {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "db_get_resource_datapoints_callback(): value is NULL");
    }

    return value;
}

static int _format_restriction_str(
        char * buf, size_t bufSize, 
        const char * field, 
        const char * value)
{
    if ((value == NULL) || (strcmp(value, "*") == 0)) {
        buf[0] = '\0';
    } else {
        int r;
        r = snprintf(buf, bufSize,
            "AND %s = '%s'",
            field, value);
        if (r >= bufSize) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                "%s(): %s restriction too long!",
                __FUNCTION__, field);
            return -1;
        }
    }

    return 0;
}

static int _format_restriction_timeval(
        char * buf, size_t bufSize, 
        const char * field_sec, 
        const char * field_usec, 
        const struct timeval *tv,
        int cmp)
{
    buf[0] = '\0';
    if (tv != NULL
    &&  (tv->tv_sec != 0 || tv->tv_usec != 0) )
    {
        int r;
        if(cmp == 0) {
            r = snprintf(
                    buf, bufSize,
                    "AND ("
                    " (%s = %ld AND %s = %ld)"
                    ")",
                    field_sec,  (long)tv->tv_sec, 
                    field_usec, (long)tv->tv_usec);
        } else if (cmp > 0 ) {
            r = snprintf(
                    buf, bufSize,
                    "AND ("
                    " %s > %ld"
                    " OR (%s = %ld AND %s >= %ld)"
                    ")",
                    field_sec,  (long)tv->tv_sec, 
                    field_sec,  (long)tv->tv_sec, 
                    field_usec, (long)tv->tv_usec);
        } else {
            r = snprintf(
                    buf, bufSize,
                    "AND ("
                    " %s < %ld"
                    " OR (%s = %ld AND %s < %ld)"
                    ")",
                    field_sec,  (long)tv->tv_sec, 
                    field_sec,  (long)tv->tv_sec, 
                    field_usec, (long)tv->tv_usec);

        }
	if (r >= bufSize) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "%s(): %s, %s restriciton is too long!",
                  __FUNCTION__, field_sec, field_usec);
	    return -1;
	}
    }

    return 0;
}

// 
// Implements db_get_events_cb_t
//
// Adds the rows to a big hierarchical structure starting with a BDM list
//
//
//
typedef struct {
    GPtrArray *list;
    int batch_size;
    int count;
    int64_t last_seq;
} make_result_list_data_t;


bdm_handle_row_status_t 
get_events_make_hab_list(
        const char * bdm_id,
        const char * hab_type,
        const char * hab_id,
        const char * node_id,
        const uint8_t node_uid[BDM_UUID_LEN],
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
        void * usr_data)
{

    make_result_list_data_t *data = (make_result_list_data_t*)usr_data;


    if(data->list == NULL) {
        data->list = g_ptr_array_new();
    }
    GPtrArray * hab_list = data->list;

    bionet_hab_t *hab;
    bionet_node_t *node = NULL;
    bionet_resource_t *resource = NULL;
    bionet_event_t *event;

    event = bionet_event_new(event_timestamp, bdm_id, event_type);

    hab = find_hab(hab_list, hab_type, hab_id);
    if (hab == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "db_get_resource_datapoints_callback(): error finding hab %s.%s", hab_type, hab_id);
	bionet_event_free(event);
        return -1;
    }
    if(event_class == _DB_GET_HAB_EVENTS) {
        bionet_hab_add_event(hab, event);
    }


    if(node_id) {
        node = find_node(hab, node_id, node_uid);
        if (node == NULL) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
                  "db_get_resource_datapoints_callback(): error finding node %s.%s.%s", 
                  bionet_hab_get_type(hab), bionet_hab_get_id(hab), node_id);
            return -1;
        }
        if(event_class == _DB_GET_NODE_EVENTS
        && data->last_seq != seq) 
        {
            data->last_seq = seq;
            bionet_node_add_event(node, event);
        }
    }

    if(resource_id) {
        resource = find_resource(node, datatype, flavor, resource_id);
        if (resource == NULL) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
                  "db_get_resource_datapoints_callback(): error finding resource %s %s %s.%s.%s:%s", 
                  bionet_resource_data_type_to_string(datatype), 
                  bionet_resource_flavor_to_string(flavor),
                  bionet_hab_get_type(hab), bionet_hab_get_id(hab), 
                  bionet_node_get_id(node), resource_id);
            return -1;
        }

        if(event_class == _DB_GET_DP_EVENTS) {
            int err = 0;
            bionet_value_t * value = _sql_value_to_bionet(stmt, column_idx, resource);
            if (value)
            {
                bionet_datapoint_t * datapoint = bionet_datapoint_new(resource, value, dp_timestamp);
                if (datapoint) {
                    bionet_resource_add_datapoint(resource, datapoint);
                    if(event_class == _DB_GET_DP_EVENTS) {
                        bionet_datapoint_add_event(datapoint, event);
                    }
                } else {
                    err++;
                }
            } else {
                err++;
            }

            if (err)
            {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
                      "db_get_resource_datapoints_callback(): error adding datapoint: %s.%s.%s:%s",
                      bionet_hab_get_type(hab),
                      bionet_hab_get_id(hab),
                      bionet_node_get_id(node),
                      bionet_resource_get_id(resource));
            }
        }
    }

    return 0;

}


bdm_handle_row_status_t 
get_events_make_bdm_list(
        const char * bdm_id,
        const char * hab_type,
        const char * hab_id,
        const char * node_id,
        const uint8_t node_uid[BDM_UUID_LEN],
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
        void * usr_data)
{

    make_result_list_data_t *data = (make_result_list_data_t*)usr_data;


    if(data->list == NULL) {
        data->list = g_ptr_array_new();
    }
    GPtrArray * bdm_list = data->list;

    bionet_bdm_t *bdm;
    bionet_hab_t *hab;
    bionet_node_t *node = NULL;
    bionet_resource_t *resource = NULL;
    bionet_event_t *event;

    event = bionet_event_new(event_timestamp, bdm_id, event_type);

    bdm = find_bdm(bdm_list, bdm_id);
    if (bdm == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "db_get_resource_datapoints_callback(): error finding bdm %s", bdm_id);
	if (event) {
	    bionet_event_free(event);
	}
        return -1;
    }

    GPtrArray * hab_list = bionet_bdm_get_hab_list(bdm);

    hab = find_hab(hab_list, hab_type, hab_id);
    if (hab == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "db_get_resource_datapoints_callback(): error finding hab %s.%s", hab_type, hab_id);
	if (event) {
	    bionet_event_free(event);
	}
        return -1;
    }
    if(event_class == _DB_GET_HAB_EVENTS) {
        bionet_hab_add_event(hab, event);
        event = NULL;
    }


    if(node_id) {
        node = find_node(hab, node_id, node_uid);
        if (node == NULL) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
                  "db_get_resource_datapoints_callback(): error finding node %s.%s.%s", 
                  bionet_hab_get_type(hab), bionet_hab_get_id(hab), node_id);
	    if (event) {
		bionet_event_free(event);
	    }
            return -1;
        }
        if(event_class == _DB_GET_NODE_EVENTS
        && data->last_seq != seq) 
        {
            data->last_seq = seq;
            bionet_node_add_event(node, event);
            event = NULL;
        }
    }

    if(resource_id) {
        resource = find_resource(node, datatype, flavor, resource_id);
        if (resource == NULL) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
                  "db_get_resource_datapoints_callback(): error finding resource %s %s %s.%s.%s:%s", 
                  bionet_resource_data_type_to_string(datatype), 
                  bionet_resource_flavor_to_string(flavor),
                  bionet_hab_get_type(hab), bionet_hab_get_id(hab), 
                  bionet_node_get_id(node), resource_id);
	    if (event) {
		bionet_event_free(event);
	    }
            return -1;
        }

        if(event_class == _DB_GET_DP_EVENTS) {
            int err = 0;
            bionet_value_t * value = _sql_value_to_bionet(stmt, column_idx, resource);
            if (value)
            {
                bionet_datapoint_t * datapoint = bionet_datapoint_new(resource, value, dp_timestamp);
                if (datapoint) {
                    bionet_resource_add_datapoint(resource, datapoint);
                    bionet_datapoint_add_event(datapoint, event);
                    event = NULL;
                } else {
                    err++;
                }
            } else {
                err++;
            }

            if (err)
            {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
                      "db_get_resource_datapoints_callback(): error adding datapoint: %s.%s.%s:%s",
                      bionet_hab_get_type(hab),
                      bionet_hab_get_id(hab),
                      bionet_node_get_id(node),
                      bionet_resource_get_id(resource));
            }
        }
    }

    if (event) {
        bionet_event_free(event);
    }

    return 0;

}



// 
// Return events for the given filter parameters by
// calling the specified callback
//
// Calls the callback for every batch_size events, and once for the
// remainder
//
// The return code of the user's callback affects how this procedure
// continues
//
int db_get_events(sqlite3* db, 
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
    int entry_end,
    db_get_event_class_t event_class,
    db_get_events_cb_t row_handler,
    void * user_data)
{
    int r;
    char sql[2048];

    char bdm_id_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    char hab_type_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    char hab_id_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    char node_id_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    char resource_id_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    char datapoint_start_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    char datapoint_end_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    char event_start_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    char event_end_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    /* The size of these is mostly fixed */
    char entry_restriction[400];

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "Looking up events from %d to %d", entry_start, entry_end);


    r = _format_restriction_str(bdm_id_restriction, sizeof(bdm_id_restriction), 
            "b.BDM_ID", bdm_id);
    if(r < 0) return -1;

    r = _format_restriction_str(hab_type_restriction, sizeof(hab_type_restriction),
            "h.HAB_Type", hab_type);
    if(r < 0) return -1;

    r = _format_restriction_str(hab_id_restriction, sizeof(hab_id_restriction),
            "h.HAB_ID", hab_id);
    if(r < 0) return -1;

    r = _format_restriction_str(node_id_restriction, sizeof(node_id_restriction),
            "n.NODE_ID", node_id);
    if(r < 0) return -1;

    r = _format_restriction_str(resource_id_restriction, sizeof(resource_id_restriction),
            "r.Resource_ID", resource_id);
    if(r < 0) return -1;

    // Datapoint Timestamps
    r = _format_restriction_timeval(datapoint_start_restriction, sizeof(datapoint_start_restriction),
            "d.Timestamp_Sec", "d.Timestamp_Usec", 
            datapoint_start, 1);
    if(r < 0) return -1;

    r = _format_restriction_timeval(datapoint_end_restriction, sizeof(datapoint_end_restriction),
            "d.Timestamp_Sec", "d.Timestamp_Usec", 
            datapoint_end, -1);
    if(r < 0) return -1;

    // Event Timetamps
    r = _format_restriction_timeval(event_start_restriction, sizeof(event_start_restriction),
            "e.timestamp_sec", "e.timestamp_usec",
            event_start, 1);
    if(r < 0) return -1;

    r = _format_restriction_timeval(event_end_restriction, sizeof(event_end_restriction),
            "e.timestamp_sec", "e.timestamp_usec",
            event_end, -1);
    if(r < 0) return -1;

    if (entry_start < 0 && entry_end < 0) {
        entry_restriction[0] = '\0';
        r = 0;
    } else if ( entry_end < 0 ){
        r = snprintf(entry_restriction, sizeof(entry_restriction),
                     " AND e.seq >= %d", entry_start);
    } else if ( entry_start < 0 ){
        r = snprintf(entry_restriction, sizeof(entry_restriction),
                     " AND e.seq <= %d", entry_end);
    } else {
        r = snprintf(entry_restriction, sizeof(entry_restriction),
                     "AND (e.seq <= %d and e.seq >= %d )",
                     entry_end, entry_start);
    }
    if (r >= sizeof(entry_restriction)) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): entry restriction is too long!", __FUNCTION__);
        return -1;
    }

    const char * fields = NULL;
    const char * tables = NULL;
    const char * where = NULL;
    const char * event_where = NULL;

    switch(event_class) {
        case _DB_GET_HAB_EVENTS:
            fields = " NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL";

            tables = " Hardware_Abstractors h";

            where = "";

            event_where = " (e.hab = h.Key) ";
            break;

        case _DB_GET_NODE_EVENTS:
            fields = 
                " n.Node_ID,"
                " n.GUID,"
                " t.Data_Type,"
                " f.Flavor,"
                " r.Resource_ID,"
                " NULL, NULL, NULL ";

            tables = 
                " Hardware_Abstractors h,"
                " Nodes n"
                "   LEFT OUTER JOIN Resources r "
                "    ON r.Node_Key=n.Key"
                "   LEFT OUTER JOIN Resource_Data_Types t "
                "    ON t.Key=r.Data_Type_Key"
                "   LEFT OUTER JOIN Resource_Flavors f"
                "    ON f.Key=r.Flavor_Key";

            where = 
                " AND n.HAB_Key=h.Key";

            event_where = " (e.node = n.key) ";
            break;

        case _DB_GET_DP_EVENTS:
            fields = 
                " n.Node_ID,"
                " n.GUID,"
                " t.Data_Type,"
                " f.Flavor,"
                " r.Resource_ID,"
                " d.Value,"
                " d.Timestamp_Sec,"
                " d.Timestamp_Usec";

            tables = 
                " Hardware_Abstractors h,"
                " Nodes n,"
                " Resources r,"
                " Resource_Data_Types t,"
                " Resource_Flavors f,"
                " Datapoints d";

            where = 
                " AND n.HAB_Key=h.Key"
                " AND r.Node_Key=n.Key"
                " AND t.Key=r.Data_Type_Key"
                " AND f.Key=r.Flavor_Key"
                " AND d.Resource_Key=r.Key";

            event_where = " (e.datapoint = d.key )";
            break;


    }

    r = snprintf(sql, sizeof(sql),
        "SELECT"
        "    h.hab_type,"
        "    h.hab_id,"
        "    %s,"
        "    e.timestamp_sec,"
        "    e.timestamp_usec,"
        "    e.seq,"
        "    b.BDM_ID,"
        "    e.islost"
        " FROM"
        "    Events e,"
        "    BDMs b,"
        "    %s"
        " WHERE"
        "    e.recording_bdm=b.Key"
        "    %s" // where
        "    AND %s" //event_where
        "    %s" //datapoint...
        "    %s"
        "    %s" //event...
        "    %s"
        "    %s" //entry
        "    %s" //bdm_id
        "    %s" // hab_type
        "    %s" //hab_id
        "    %s" //node_id
        "    %s" //resource_id
        " ORDER BY"
        "     e.seq ASC",
            // Fields
            fields, 
            // Tables
            tables, 
            // Where
            where, 
            event_where, 
            datapoint_start_restriction,
            datapoint_end_restriction,
            event_start_restriction,
            event_end_restriction,
            entry_restriction,
            bdm_id_restriction,
            hab_type_restriction,
            hab_id_restriction,
            node_id_restriction,
            resource_id_restriction
        );

    if (r >= sizeof(sql)) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "db_get_events(): SQL doesnt fit in buffer!");
        return -1;
    }

#ifdef _DUMP_SQL
    const char * type = "";
    switch(event_class) {
        case _DB_GET_HAB_EVENTS:
            type = "HAB";
            break;

        case _DB_GET_NODE_EVENTS:
            type = "NODE";
            break;

        case _DB_GET_DP_EVENTS:
            type = "DP";
            break;
    }
    g_log(
        BDM_LOG_DOMAIN,
        G_LOG_LEVEL_DEBUG,
        "db_get_events(%s): SQL is %s",
        type, sql
    );
#endif

    sqlite3_stmt *stmt;
    int row_count = 0;
    r = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (r != SQLITE_OK) goto db_fail;

    for(;;) {
        r = sqlite3_step(stmt);
        if(r == SQLITE_BUSY) {
            g_usleep(20 * 1000);
            continue;
        }
        if(r == SQLITE_ROW) {
            struct timeval row_dp_timestamp = {0,0};
            struct timeval row_event_timestamp = {0,0};
            bdm_handle_row_status_t status;
            row_count++;

            const char * row_habtype_id  = (const char *)sqlite3_column_text(stmt, 0); 
            const char * row_habname_id  = (const char *)sqlite3_column_text(stmt, 1); 
            const char * row_node_id     = (const char *)sqlite3_column_text(stmt, 2); 
            const uint8_t * row_node_uid   = (const uint8_t *)sqlite3_column_blob(stmt, 3); 
            int row_node_guid_len        = sqlite3_column_bytes(stmt, 3); 
            const char * row_datatype    = (const char *)sqlite3_column_text(stmt, 4); 
            const char * row_flavor      = (const char *)sqlite3_column_text(stmt, 5); 
            const char * row_resource_id = (const char *)sqlite3_column_text(stmt, 6); 
            static const int row_i_value =                                         7; 
            row_dp_timestamp.tv_sec      =               sqlite3_column_int (stmt, 8);
            row_dp_timestamp.tv_usec     =               sqlite3_column_int (stmt, 9);
            row_event_timestamp.tv_sec   =               sqlite3_column_int(stmt, 10);
            row_event_timestamp.tv_usec  =               sqlite3_column_int(stmt, 11);
            sqlite_int64 row_event_seq  =               sqlite3_column_int64(stmt, 12);
            const char * row_bdm_id      = (const char *)sqlite3_column_text(stmt, 13); 
            int row_islost               =               sqlite3_column_int(stmt, 14); 

            bionet_event_type_t event_type = row_islost?BIONET_EVENT_LOST:BIONET_EVENT_PUBLISHED;

            if(row_node_uid && row_node_guid_len != BDM_UUID_LEN) {
                break;
            }

            status = row_handler(
                    row_bdm_id,
                    row_habtype_id,
                    row_habname_id,
                    row_node_id, row_node_uid,
                    row_datatype?bionet_resource_data_type_from_string(row_datatype):-1, 
                    row_flavor?bionet_resource_flavor_from_string(row_flavor):-1, 
                    row_resource_id,
                    &row_dp_timestamp,
                    event_class, event_type, &row_event_timestamp, row_event_seq,
                    row_i_value, stmt, user_data);

            if(BDM_HANDLE_OK == status) {
                continue;
            }
        }
        break;
    }
    if (r != SQLITE_DONE) goto db_fail;

    sqlite3_finalize(stmt);

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "%s(): Found %d matching events from sequence %d to %d", __FUNCTION__,
            row_count, entry_start, entry_end);

    return 0;

    db_fail:
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            "db_get_events(): SQL error: %s", sqlite3_errmsg(db));
        g_log(
            BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "db_get_events(): SQL was: %s",
            sql
        );
        sqlite3_reset(stmt);
        sqlite3_finalize(stmt);
        return -1;

}

// 
// Finds all matching datapoints, and returns them as a GPtrArray of bionet_hab_t.
// Use hab_list_free to free the list returned
//
GPtrArray *db_get_habs(
    sqlite3 *db,
    const char *bdm_id,
    const char *hab_type,
    const char *hab_id,
    const struct timeval *event_start,
    const struct timeval *event_end,
    int entry_start,
    int entry_end)
{
    int r;
    make_result_list_data_t data = {0};
    data.last_seq = -1;

    // We want to return an empty list if 
    // no results, NULL on error
    data.list = g_ptr_array_new();

    r = db_get_events(db, 
            bdm_id,
            hab_type,
            hab_id,
            NULL,
            NULL,
            NULL,
            NULL,
            event_start, event_end,
            entry_start, entry_end,
            _DB_GET_HAB_EVENTS,
            get_events_make_hab_list,
            (void*)&data);

    if(r == 0) {
        return data.list;
    } else {
        if(data.list){
            hab_list_free(data.list);
        }
        return NULL;
    }

}


// 
// Finds all matching datapoints, and returns them as a GPtrArray of bionet_hab_t.
// Use hab_list_free to free the list returned
//
GPtrArray *db_get_nodes(
    sqlite3 *db,
    const char *bdm_id,
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const struct timeval *event_start, 
    const struct timeval *event_end,
    int entry_start,
    int entry_end)
{
    int r;
    make_result_list_data_t data = {0};
    data.last_seq = -1;

    // We want to return an empty list if 
    // no results, NULL on error
    data.list = g_ptr_array_new();

    r = db_get_events(db, 
            bdm_id,
            hab_type,
            hab_id,
            node_id,
            NULL,
            NULL, NULL,
            event_start, event_end,
            entry_start, entry_end,
            _DB_GET_NODE_EVENTS,
            get_events_make_hab_list,
            (void*)&data);

    if(r == 0) {
        return data.list;
    } else {
        if(data.list){
            hab_list_free(data.list);
        }
        return NULL;
    }

}

// 
// Finds all matching datapoints, and returns them as a GPtrArray of bionet_bdm_t.
// Use bdm_list_free to free the list returned
//
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
    int entry_end)
{
    int r;
    make_result_list_data_t data = {0};
    data.last_seq = -1;

    // We want to return an empty list if 
    // no results, NULL on error
    data.list = g_ptr_array_new();

    r = db_get_events(db, 
            bdm_id,
            hab_type,
            hab_id,
            node_id,
            resource_id,
            datapoint_start,
            datapoint_end,
            event_start, event_end,
            entry_start,
            entry_end,
            _DB_GET_DP_EVENTS,
            get_events_make_bdm_list,
            (void*)&data);

    if(r == 0) {
        return data.list;
    } else {
        if(data.list){
            hab_list_free(data.list);
        }
        return NULL;
    }

}
// 
// Finds all matching datapoints, and returns them as a GPtrArray of bionet_hab_t.
// Use hab_list_free to free the list returned
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
    int entry_end)
{
    int r;
    make_result_list_data_t data = {0};
    data.last_seq = -1;

    // We want to return an empty list if 
    // no results, NULL on error
    data.list = g_ptr_array_new();

    r = db_get_events(db, 
            bdm_id,
            hab_type,
            hab_id,
            node_id,
            resource_id,
            datapoint_start,
            datapoint_end,
            event_start, event_end,
            entry_start,
            entry_end,
            _DB_GET_DP_EVENTS,
            get_events_make_hab_list,
            (void*)&data);

    if(r == 0) {
        return data.list;
    } else {
        if(data.list){
            hab_list_free(data.list);
        }
        return NULL;
    }

}

// 
// Finds all matching metadata, and returns them as a GPtrArray of bionet_bdm_t.
// Use hab_list_free to free the list returned
//
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
    int entry_end)
{
    int r;
    make_result_list_data_t data = {0};
    data.last_seq = -1;

    // We want to return an empty list if 
    // no results, NULL on error
    data.list = g_ptr_array_new();


    r = db_get_events(db, 
            bdm_id,
            hab_type,
            hab_id,
            NULL,
            NULL,
            NULL,
            NULL,
            event_start, event_end,
            entry_start, entry_end,
            _DB_GET_HAB_EVENTS,
            get_events_make_bdm_list,
            (void*)&data);
    if ( r != 0 ) goto cleanup;

    r = db_get_events(db, 
            bdm_id,
            hab_type,
            hab_id,
            node_id,
            NULL,
            NULL, NULL,
            event_start, event_end,
            entry_start, entry_end,
            _DB_GET_NODE_EVENTS,
            get_events_make_bdm_list,
            (void*)&data);
    if ( r != 0 ) goto cleanup;

    return data.list;

    cleanup:
        bdm_list_free(data.list);
        return NULL;
}

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
    int entry_end)
{
    int r;
    make_result_list_data_t data = {0};
    data.last_seq = -1;

    // We want to return an empty list if 
    // no results, NULL on error
    data.list = g_ptr_array_new();


    r = db_get_events(db, 
            bdm_id,
            hab_type,
            hab_id,
            NULL,
            NULL,
            NULL,
            NULL,
            event_start, event_end,
            entry_start, entry_end,
            _DB_GET_HAB_EVENTS,
            get_events_make_bdm_list,
            (void*)&data);
    if ( r != 0 ) goto cleanup;

    r = db_get_events(db, 
            bdm_id,
            hab_type,
            hab_id,
            node_id,
            NULL,
            NULL, 
            NULL,
            event_start, event_end,
            entry_start, entry_end,
            _DB_GET_NODE_EVENTS,
            get_events_make_bdm_list,
            (void*)&data);
    if ( r != 0 ) goto cleanup;

    r = db_get_events(db, 
            bdm_id,
            hab_type,
            hab_id,
            node_id,
            resource_id,
            NULL, 
            NULL,
            event_start, event_end,
            entry_start, entry_end,
            _DB_GET_DP_EVENTS,
            get_events_make_bdm_list,
            (void*)&data);
    if ( r != 0 ) goto cleanup;

    return data.list;

    cleanup:
        bdm_list_free(data.list);
        return NULL;
}

// 
// Finds all matching metadata, and returns them as a GPtrArray of bionet_hab_t.
// Use hab_list_free to free the list returned
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
    int entry_end)
{
    int r;
    make_result_list_data_t data = {0};
    data.last_seq = -1;

    // We want to return an empty list if 
    // no results, NULL on error
    data.list = g_ptr_array_new();


    r = db_get_events(db, 
            bdm_id,
            hab_type,
            hab_id,
            NULL,
            NULL,
            NULL,
            NULL,
            event_start, event_end,
            entry_start, entry_end,
            _DB_GET_HAB_EVENTS,
            get_events_make_hab_list,
            (void*)&data);
    if ( r != 0 ) goto cleanup;

    r = db_get_events(db, 
            bdm_id,
            hab_type,
            hab_id,
            node_id,
            NULL,
            NULL, NULL,
            event_start, event_end,
            entry_start, entry_end,
            _DB_GET_NODE_EVENTS,
            get_events_make_hab_list,
            (void*)&data);
    if ( r != 0 ) goto cleanup;

    return data.list;

    cleanup:
        hab_list_free(data.list);
        return NULL;
}

// If the single column is NULL, don't do anything
static int db_set_int_callback(
    void *sql_ret_void,
    int argc,
    char **argv,
    char **azColName)
{
    if (argc == 1) {
        if(argv[0]) {
            *(int*)sql_ret_void = atoi(argv[0]);
        }
        return 0;
    } else {
        int i;

        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s(): Error interpreting SELECT Results",
                __FUNCTION__);
        for(i=0;i<argc;i++){
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                    "    column[%d] '%s' = %s", i, azColName[i], argv[i]);
        }
    }

    return -1;
}


int db_get_latest_entry_seq(sqlite3 *db) {
    int seq = -1;
    int r;
    char * zErrMsg = NULL;

    r = sqlite3_exec(
        db,
        "SELECT max(seq) as Num from Events",
        db_set_int_callback,
        &seq,
        &zErrMsg
    );
    if (r != SQLITE_OK) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): SELECT SQL error: %s",
            __FUNCTION__, zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }

    if(seq<0) {
        seq = 0;
    }
    return seq;
}


static bionet_datapoint_t * _dbrow_to_bionet_dp(
        const char * bdm_id,
        const char * hab_type,
        const char * hab_id,
        const char * node_id,
        const uint8_t node_uid[BDM_UUID_LEN],
        bionet_resource_data_type_t datatype,
        bionet_resource_flavor_t flavor,
        const char * resource_id,
        const struct timeval *dp_timestamp,
        bionet_event_type_t event_type,
        const struct timeval *event_timestamp,
        const int column_idx,
        sqlite3_stmt * stmt
        )
{
    bionet_hab_t *hab = NULL;
    bionet_node_t *node = NULL;
    bionet_resource_t *resource = NULL;
    bionet_datapoint_t *datapoint = NULL;
    bionet_value_t *value = NULL;
    bionet_event_t *event = NULL;

    if(NULL == bdm_id 
    || NULL == hab_type
    || NULL == hab_id
    || NULL == node_id
    || NULL == node_uid
    || NULL == resource_id
    ) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
                "%s(): Missing parameter", __FUNCTION__);
        return NULL;
    }

    event = bionet_event_new(event_timestamp, bdm_id, event_type);
    if (event == NULL) goto fail0;

    hab = bionet_hab_new(hab_type, hab_id);
    if (hab == NULL) goto fail1;

    node = find_node(hab, node_id, node_uid);
    if (node == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
              "db_get_resource_datapoints_callback(): error finding node %s.%s.%s", 
              bionet_hab_get_type(hab), bionet_hab_get_id(hab), node_id);
        goto fail2;
    }

    resource = find_resource(node, datatype, flavor, resource_id);
    if (resource == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
              "db_get_resource_datapoints_callback(): error finding resource %s %s %s.%s.%s:%s", 
              bionet_resource_data_type_to_string(datatype), 
              bionet_resource_flavor_to_string(flavor),
              bionet_hab_get_type(hab), bionet_hab_get_id(hab), 
              bionet_node_get_id(node), resource_id);
        goto fail2;
    }

    value = _sql_value_to_bionet(stmt, column_idx, resource);
    if( NULL == value) goto fail2;

    datapoint = bionet_datapoint_new(resource, value, dp_timestamp);
    if( NULL == datapoint) goto fail2;

    bionet_resource_add_datapoint(resource, datapoint);
    bionet_datapoint_add_event(datapoint, event);

    return datapoint;

fail2:
    bionet_hab_free(hab);

fail1:
    bionet_event_free(event);

fail0:
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
          "%s(): error making datapoint: %s.%s.%s:%s",
          __FUNCTION__,
          hab_type, hab_id, node_id, resource_id);

    return NULL;
}

static bdm_handle_row_status_t 
get_events_publish_dp(
        const char * bdm_id,
        const char * hab_type,
        const char * hab_id,
        const char * node_id,
        const uint8_t node_uid[BDM_UUID_LEN],
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
        void * usr_data)
{
    int r, i;
    bdm_db_batch_t * tmp_dbb = (bdm_db_batch_t*)usr_data;

    if(event_class != _DB_GET_DP_EVENTS) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR,
              "%s(): usage error: wrong query class", __FUNCTION__);
    }


    bionet_datapoint_t * datapoint = _dbrow_to_bionet_dp(
        bdm_id, hab_type, hab_id, node_id, node_uid,
        datatype, flavor, resource_id,
        dp_timestamp,
        event_type, event_timestamp,
        column_idx, stmt);

    if(NULL == datapoint) {
        return BDM_HANDLE_ERR;
    }

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
          "%s(): publishing %d datapoint events for %s.%s.%s:%s", __FUNCTION__,
          bionet_datapoint_get_num_events(datapoint),
          hab_type, hab_id, node_id, resource_id);

    for ( i=0; i< bionet_datapoint_get_num_events(datapoint); i++ ) {
        bionet_event_t * event = bionet_datapoint_get_event_by_index(datapoint, i);
        dbb_event_t * dbb_event;

        dbb_bionet_event_data_t bionet_ptr;
        bionet_ptr.datapoint = datapoint;

        dbb_event = dbb_add_event(tmp_dbb, DBB_DATAPOINT_EVENT, 
                bionet_ptr, bdm_id, event_timestamp);

        bionet_event_t * tmp_event = bionet_event_new(
                bionet_event_get_timestamp(event),
                bionet_event_get_bdm_id(event),
                bionet_event_get_type(event));
        r = bdm_report_datapoint(dbb_event, tmp_event);
        if(r != 0) {
            return BDM_HANDLE_ERR;
        }
    }

    bionet_hab_free(bionet_datapoint_get_hab(datapoint));

    return BDM_HANDLE_OK;
}

int db_publish_synced_datapoints(
        sqlite3 *db,
        sqlite_int64 first_seq, 
        sqlite_int64 last_seq)
{
    int r;

    bdm_db_batch_t * tmp_dbb = calloc(1, sizeof(bdm_db_batch_t));
    if(tmp_dbb == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): Out of Memory!", __FUNCTION__);
        return -1;
    }

    r = db_get_events(db, 
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL, NULL,
            first_seq,
            last_seq,
            _DB_GET_DP_EVENTS,
            get_events_publish_dp,
            (void*)tmp_dbb);

    dbb_free(tmp_dbb);

    if(r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
              "%s(): error publishing synced dataopints (seq %lld-%lld)",
              __FUNCTION__, first_seq, last_seq);
        return -1;
    }

    db_save_dangling_datapoints(db, first_seq, last_seq);

    return 0;
}

int db_save_dangling_datapoints(
        sqlite3 *db,
        sqlite_int64 first_seq, 
        sqlite_int64 last_seq)
{
    int r;

    const char * sql = 
        "INSERT INTO DanglingDatapoints (datapoint) "
          "SELECT d.key "
          "FROM events e "
          "JOIN Datapoints d "
             " ON e.datapoint=d.Key "
          "WHERE "
            "e.seq >= ? AND e.seq <= ? "
            "AND d.resource_key NOT IN "
               "(SELECT r.key from Resources r) ";

#ifdef _DUMP_SQL
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
        "%s(): SQL is %s", __FUNCTION__, sql);
#endif

    if(all_stmts[SAVE_DANGLING_DATAPOINTS]  == NULL) {
	r = sqlite3_prepare_v2(db, sql,
	    -1, &all_stmts[SAVE_DANGLING_DATAPOINTS] , NULL);

	if (r != SQLITE_OK) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s(): SQL prepare error: %s", __FUNCTION__, sqlite3_errmsg(db));
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                "SQL is \n%s", sql);
            return -1;
	}

    }

    sqlite3_stmt * this_stmt = all_stmts[SAVE_DANGLING_DATAPOINTS];

    int param = 1;
    r = sqlite3_bind_int64(this_stmt, param++, first_seq);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s() SQL bind error", __FUNCTION__);
	return -1;
    }

    r = sqlite3_bind_int64(this_stmt, param, last_seq);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s() SQL bind error", __FUNCTION__);
	return -1;
    }

    while(SQLITE_BUSY == (r = sqlite3_step(this_stmt))){
        g_usleep(20 * 1000);
    }
        
    int ret = 0;
    switch (r) {
        case SQLITE_DONE:
        case SQLITE_ROW:
            // Success!
            break;

        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s() SQL error: %s", 
                    __FUNCTION__, sqlite3_errmsg(db));
            ret = -1;
    }

    sqlite3_reset(this_stmt);
    sqlite3_clear_bindings(this_stmt);

    return ret;
}

static void delete_dangling_datapoints(
        sqlite3 *db,
        sqlite_int64 * keys, 
        int count)
{
    int r;
    int i;

    if(all_stmts[CLEAN_DANGLING_STMT] == NULL) {
	r = sqlite3_prepare_v2(db, 
            "DELETE "
            "FROM DanglingDatapoints "
            "WHERE datapoint = ? ",
	    -1, &all_stmts[CLEAN_DANGLING_STMT], NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s() SQL error: %s", 
                    __FUNCTION__, sqlite3_errmsg(db));
	    return;
	}
    }
    sqlite3_stmt * this_stmt = all_stmts[CLEAN_DANGLING_STMT];

    for(i=0; i<count; i++) {
        r = sqlite3_bind_int64(this_stmt, 1, keys[i]);
        if(r != SQLITE_OK){
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s() SQL bind error", __FUNCTION__);
            return;
        }


        while(SQLITE_BUSY == (r = sqlite3_step(this_stmt))){
            g_usleep(20 * 1000);
        }
            
        switch (r) {
            case SQLITE_DONE:
            case SQLITE_ROW:
                // Success!
                break;

            default:
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s() SQL error: %s", 
                        __FUNCTION__, sqlite3_errmsg(db));
        }

        sqlite3_reset(this_stmt);
    }

}

// This publishs datapoints that can be parsed
// with the new metadata
int db_publish_sync_affected_datapoints(
        sqlite3 *db,
        sqlite_int64 first_seq, 
        sqlite_int64 last_seq)
{
    int r;
    bdm_db_batch_t * tmp_dbb = calloc(1, sizeof(bdm_db_batch_t));
    if(tmp_dbb == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s(): Out of Memory!", __FUNCTION__);
        return -1;
    }

    // We call this callback same as db_get_events()
    db_get_events_cb_t row_handler = get_events_publish_dp;
    void * user_data = tmp_dbb;
    db_get_event_class_t event_class = _DB_GET_DP_EVENTS;

    const char * sql = 
        "SELECT"
            " h.hab_type,"
            " h.hab_id,"
            " n.Node_ID,"
            " n.GUID,"
            " t.Data_Type,"
            " f.Flavor,"
            " r.Resource_ID,"
            " d.Value,"
            " d.Timestamp_Sec,"
            " d.Timestamp_Usec,"
            " e.timestamp_sec,"
            " e.timestamp_usec,"
            " e.seq,"
            " b.BDM_ID,"
            " e.islost,"
            " d.key"
        " FROM"
            " Datapoints d"
            " JOIN DanglingDatapoints dd ON d.key=dd.datapoint "
            " JOIN Events e ON e.datapoint=d.key "
            " JOIN Resources r ON r.key=d.Resource_key"
            " JOIN Resource_Data_Types t ON t.Key=r.Data_Type_Key"
            " JOIN Resource_Flavors f ON f.key=r.Flavor_key"
            " JOIN Nodes n ON n.key=r.Node_Key "
            " JOIN Hardware_Abstractors h ON h.key = n.HAB_Key "
            " JOIN BDMs b ON b.key=e.recording_bdm "
        " ORDER BY"
        "     e.seq ASC";

#ifdef _DUMP_SQL
    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
        "%s(): SQL is %s", __FUNCTION__, sql);
#endif

    if(all_stmts[GET_SYNC_AFFECTED_DATAPOINTS]  == NULL) {
	r = sqlite3_prepare_v2(db, sql,
	    -1, &all_stmts[GET_SYNC_AFFECTED_DATAPOINTS] , NULL);

	if (r != SQLITE_OK) {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s(): SQL prepare error: %s", __FUNCTION__, sqlite3_errmsg(db));
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                "SQL is \n%s", sql);
            dbb_free(tmp_dbb);
            return -1;
	}

    }


    sqlite3_stmt * this_stmt = all_stmts[GET_SYNC_AFFECTED_DATAPOINTS];

    /*
    int param = 1;
    r = sqlite3_bind_int64(this_stmt, param++, first_seq);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s() SQL bind error (%d)", __FUNCTION__, param);
        dbb_free(tmp_dbb);
	return -1;
    }

    r = sqlite3_bind_int64(this_stmt, param++, last_seq);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s() SQL bind error (%d)", __FUNCTION__, param);
        dbb_free(tmp_dbb);
	return -1;
    }
    */

    int row_count = 0;
    sqlite_int64 *published_keys = NULL;
    int num_published_keys = 0;
    int published_keys_size = 0;

    for(;;) {
        r = sqlite3_step(this_stmt);
        if(r == SQLITE_BUSY) {
            g_usleep(20 * 1000);
            continue;
        }
        if(r == SQLITE_ROW) {
            struct timeval row_dp_timestamp = {0,0};
            struct timeval row_event_timestamp = {0,0};
            bdm_handle_row_status_t status;
            row_count++;

            const char * row_habtype_id  = (const char *)sqlite3_column_text(this_stmt, 0); 
            const char * row_habname_id  = (const char *)sqlite3_column_text(this_stmt, 1); 
            const char * row_node_id     = (const char *)sqlite3_column_text(this_stmt, 2); 
            const uint8_t * row_node_uid   = (const uint8_t *)sqlite3_column_blob(this_stmt, 3); 
            int row_node_guid_len        = sqlite3_column_bytes(this_stmt, 3); 
            const char * row_datatype    = (const char *)sqlite3_column_text(this_stmt, 4); 
            const char * row_flavor      = (const char *)sqlite3_column_text(this_stmt, 5); 
            const char * row_resource_id = (const char *)sqlite3_column_text(this_stmt, 6); 
            static const int row_i_value =                                         7; 
            row_dp_timestamp.tv_sec      =               sqlite3_column_int (this_stmt, 8);
            row_dp_timestamp.tv_usec     =               sqlite3_column_int (this_stmt, 9);
            row_event_timestamp.tv_sec   =               sqlite3_column_double(this_stmt, 10);
            row_event_timestamp.tv_usec  =               sqlite3_column_double(this_stmt, 11);
            sqlite_int64 row_event_seq  =               sqlite3_column_int64(this_stmt, 12);
            const char * row_bdm_id      = (const char *)sqlite3_column_text(this_stmt, 13); 
            int row_islost               =               sqlite3_column_int(this_stmt, 14); 
            sqlite_int64 dp_key          =               sqlite3_column_int64(this_stmt, 15); 

            bionet_event_type_t event_type = row_islost?BIONET_EVENT_LOST:BIONET_EVENT_PUBLISHED;

            if(row_node_uid && row_node_guid_len != BDM_UUID_LEN) {
                break;
            }

            status = row_handler(
                    row_bdm_id,
                    row_habtype_id,
                    row_habname_id,
                    row_node_id, row_node_uid,
                    row_datatype?bionet_resource_data_type_from_string(row_datatype):-1, 
                    row_flavor?bionet_resource_flavor_from_string(row_flavor):-1, 
                    row_resource_id,
                    &row_dp_timestamp,
                    event_class, event_type, &row_event_timestamp, row_event_seq,
                    row_i_value, this_stmt, user_data);

            if(BDM_HANDLE_OK == status) {
                if(num_published_keys >= published_keys_size) {
                    published_keys_size += 32;
                    published_keys = realloc(published_keys, published_keys_size * sizeof(sqlite_int64));
                    if( NULL == published_keys) {
                        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "out of memory");
                        published_keys_size = 0;
                        num_published_keys = 0;
                        continue;
                    }
                }

                published_keys[num_published_keys] = dp_key;
                num_published_keys++;
                continue;
            }
        }
        break;
    }
    if (r != SQLITE_DONE) goto db_fail;

    sqlite3_reset(this_stmt);
    sqlite3_clear_bindings(this_stmt);

    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "%s(): Published %d/%d datapoints affected by sync [%lld,%lld]", 
            __FUNCTION__, row_count, num_published_keys, first_seq, last_seq);

    dbb_free(tmp_dbb);

    if(row_count) {
        delete_dangling_datapoints(db, published_keys, num_published_keys);
    }
    free(published_keys);

    return 0;

    db_fail:

        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            "%s(): SQL error: %s", __FUNCTION__, sqlite3_errmsg(db));
        sqlite3_reset(this_stmt);
        sqlite3_clear_bindings(this_stmt);
        dbb_free(tmp_dbb);
        return -1;
}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
