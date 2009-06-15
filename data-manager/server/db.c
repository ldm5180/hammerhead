
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#define _ISOC99_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sqlite3.h>
#include <openssl/sha.h>
#include <sys/time.h>
#include <glib.h>

#include <bionet.h>

#include "bdm-util.h"
#include "bionet-data-manager.h"


static int add_bdm_to_db(const char *bdm_id);


static sqlite3 *db = NULL;

static sqlite3_stmt * insert_hab_stmt = NULL;
static sqlite3_stmt * insert_node_stmt = NULL;
static sqlite3_stmt * insert_resource_stmt = NULL;
static sqlite3_stmt * insert_datapoint_stmt = NULL;
static sqlite3_stmt * insert_datapoint_sync_stmt = NULL;
static sqlite3_stmt * insert_bdm_stmt = NULL;

extern char * database_file;
char bdm_id[256] = { 0 };

typedef struct sql_return {
    GPtrArray *hab_list;
    struct timeval * latest_entry;
} sql_return_t;

int db_init(void) {
    int r;

    r = sqlite3_open(database_file, &db);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error opening database %s: %s\n", database_file, sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    r = sqlite3_busy_timeout(db, 5 * 1000);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error setting busy timeout on database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    // Add self as bdm to db
    add_bdm_to_db(bdm_id);

    return 0;
}


// 
// shut down the database
//

void db_shutdown(void) {

    if(insert_datapoint_stmt){
	sqlite3_finalize(insert_datapoint_stmt);
	insert_datapoint_stmt = NULL;
    }
    if(insert_datapoint_sync_stmt){
	sqlite3_finalize(insert_datapoint_sync_stmt);
	insert_datapoint_sync_stmt = NULL;
    }
    if(insert_bdm_stmt){
	sqlite3_finalize(insert_bdm_stmt);
	insert_bdm_stmt = NULL;
    }

    sqlite3_close(db);
}




// this calls COMMIT and retries indefinately if the DB is busy
static int do_commit(void) {
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

        if (r == SQLITE_OK) return 0;

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


static double timevaltodouble(struct timeval *tv) {
    return (double)tv->tv_sec + (1e-6 * tv->tv_usec); 
}

static double gettimedouble(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return timevaltodouble(&tv);
}

static void doubletotimeval(double t, struct timeval *tv) {
    tv->tv_sec = (time_t)t;
    tv->tv_usec = (suseconds_t)(t-tv->tv_sec * 1e6);
} 

// 
// database back-end internal helper functions
//
// Each of these functions does a single SQL INSERT.
// Returns 0 on success, -1 on failure.
//


static int add_hab_to_db(const bionet_hab_t *hab) {
    int r;

    if(insert_hab_stmt == NULL) {
	r = sqlite3_prepare_v2(db, 
            "INSERT"
            " OR IGNORE"
            " INTO Hardware_Abstractors (HAB_Type, HAB_ID, Entry_Timestamp) "
            " VALUES (?,?,?)",
	    -1, &insert_hab_stmt, NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-hab SQL error: %s\n", sqlite3_errmsg(db));
	    return -1;
	}
    }

    double entry = gettimedouble();

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

    r = sqlite3_bind_double( insert_hab_stmt, param++,  entry);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-hab SQL bind error");
	return -1;
    }

    while(SQLITE_BUSY == (r = sqlite3_step(insert_hab_stmt)));
    if (r != SQLITE_DONE) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint SQL error: %s\n", sqlite3_errmsg(db));
	sqlite3_reset(insert_hab_stmt);
        return -1;
    }
    sqlite3_reset(insert_hab_stmt);
    sqlite3_clear_bindings(insert_hab_stmt);

    return 0;
}




static int add_node_to_db(const bionet_node_t *node) {
    int r;

    if(insert_node_stmt == NULL) {
	r = sqlite3_prepare_v2(db, 
            "INSERT"
            " OR IGNORE"
            " INTO Nodes (HAB_Key, Node_ID, Entry_Timestamp)"
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

    double entry = gettimedouble();

    int param = 1;
    r = sqlite3_bind_text(insert_node_stmt, param++, bionet_node_get_id(node), -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-node SQL bind error");
	return -1;
    }

    r = sqlite3_bind_double( insert_node_stmt, param++,  entry);
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

    while(SQLITE_BUSY == (r = sqlite3_step(insert_node_stmt)));
    if (r != SQLITE_DONE) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-node SQL error: %s\n", sqlite3_errmsg(db));
	sqlite3_reset(insert_node_stmt);
        return -1;
    }
    sqlite3_reset(insert_node_stmt);
    sqlite3_clear_bindings(insert_node_stmt);

    return 0;
}




static int add_resource_to_db(bionet_resource_t *resource) {
    int r;

    SHA_CTX sha_ctx;
    uint8_t byte;
    unsigned char sha_digest[SHA_DIGEST_LENGTH];

    const char * hab_type;
    const char * hab_id;
    const char * node_id;
    const char * resource_id;

    r = SHA1_Init(&sha_ctx);
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error initializing SHA1 context\n");
        return -1;
    }

    hab_type = bionet_hab_get_type(bionet_resource_get_hab(resource));
    r = SHA1_Update(&sha_ctx, hab_type, strlen(hab_type));
    if (r != 1) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource HAB-Type\n");
	return -1;
    }
    
    hab_id = bionet_hab_get_id(bionet_resource_get_hab(resource));
    r = SHA1_Update(&sha_ctx, hab_id, strlen(hab_id));
    if (r != 1) {
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource HAB-ID\n");
	return -1;
    }

    node_id = bionet_node_get_id(bionet_resource_get_node(resource));
    r = SHA1_Update(&sha_ctx, node_id, strlen(node_id));
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource Node-ID\n");
        return -1;
    }

    resource_id = bionet_resource_get_id(resource);
    r = SHA1_Update(&sha_ctx, resource_id, strlen(resource_id));
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource ID\n");
        return -1;
    }

    byte = bionet_resource_get_data_type(resource);
    r = SHA1_Update(&sha_ctx, &byte, 1);
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource Data Type\n");
        return -1;
    }

    byte = bionet_resource_get_flavor(resource);
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

    double entry = gettimedouble();

    int param = 1;
    r = sqlite3_bind_blob(insert_resource_stmt, param++, sha_digest, BDM_RESOURCE_KEY_LENGTH, SQLITE_TRANSIENT);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL bind error");
	return -1;
    }
    r = sqlite3_bind_text(insert_resource_stmt, param++, resource_id, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL bind error");
	return -1;
    }
    r = sqlite3_bind_double( insert_resource_stmt, param++,  entry);
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
    r = sqlite3_bind_text(insert_resource_stmt, param++, bionet_resource_data_type_to_string(bionet_resource_get_data_type(resource)), -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL bind error");
	return -1;
    }
    r = sqlite3_bind_text(insert_resource_stmt, param++, bionet_resource_flavor_to_string(bionet_resource_get_flavor(resource)), -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL bind error");
	return -1;
    }

    while(SQLITE_BUSY == (r = sqlite3_step(insert_resource_stmt)));
    if (r != SQLITE_DONE) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL error: %s\n", sqlite3_errmsg(db));
	sqlite3_reset(insert_resource_stmt);
        return -1;
    }
    sqlite3_reset(insert_resource_stmt);
    sqlite3_clear_bindings(insert_resource_stmt);

    return 0;
}


static int add_bdm_to_db(const char *bdm_id) {
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

    while(SQLITE_BUSY == (r = sqlite3_step(insert_bdm_stmt)));
    if (r != SQLITE_DONE) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-bdm SQL error: %s\n", sqlite3_errmsg(db));
	sqlite3_reset(insert_bdm_stmt);
        return -1;
    }
    sqlite3_reset(insert_bdm_stmt);
    sqlite3_clear_bindings(insert_bdm_stmt);

    return 0;
}


static int add_datapoint_to_db(bionet_datapoint_t *datapoint) {
    int r;

    if(insert_datapoint_stmt == NULL) {
	r = sqlite3_prepare_v2(db, 
	    "INSERT"
	    " OR IGNORE"
	    " INTO Datapoints"
	    " SELECT"
	    "     NULL, Resources.Key, BDMs.Key, ?, ?, ?, ?"
	    "     FROM Hardware_Abstractors, Nodes, Resources, BDMs"
	    "     WHERE"
	    "         Hardware_Abstractors.HAB_Type = ?"
	    "         AND Hardware_Abstractors.HAB_ID = ?"
	    "         AND Nodes.HAB_Key = Hardware_Abstractors.Key"
	    "         AND Nodes.Node_ID = ?"
	    "         AND Resources.Node_Key = Nodes.Key"
	    "         AND Resources.Resource_ID = ?"
	    "         AND BDMs.BDM_ID = ?"
	    ";",
	    -1, &insert_datapoint_stmt, NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint SQL error: %s\n", sqlite3_errmsg(db));
	    return -1;
	}
    }

    // FIXME: might be the wrong resource (might differ in data type or flavor)
    bionet_value_t *value = bionet_datapoint_get_value(datapoint);
    struct timeval * timestamp = bionet_datapoint_get_timestamp(datapoint);
    double entry = gettimedouble();

    bionet_resource_t * resource = bionet_value_get_resource(value);
    bionet_node_t * node = bionet_resource_get_node(resource);
    bionet_hab_t * hab = bionet_node_get_hab(node);

//    fprintf(stderr, "%s,%s,%s\n", bionet_datapoint_timestamp_to_string(datapoint),
//	    bionet_resource_get_name(resource),
//	    bionet_value_to_str(value));

    // Bind host variables to the prepared statement -- This eliminates the need to escape strings
    // In order of the placeholders (?) in the SQL
    int param = 1;
    r = sqlite3_bind_text(insert_datapoint_stmt, param++, bionet_value_to_str(value), -1, free);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint SQL bind error");
	return -1;
    }
    r = sqlite3_bind_int( insert_datapoint_stmt, param++,  timestamp->tv_sec);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint SQL bind error");
	return -1;
    }
    r = sqlite3_bind_int( insert_datapoint_stmt, param++,  timestamp->tv_usec);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint SQL bind error");
	return -1;
    }
    r = sqlite3_bind_double( insert_datapoint_stmt, param++,  entry);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint SQL bind error");
	return -1;
    }
    r = sqlite3_bind_text(insert_datapoint_stmt, param++, bionet_hab_get_type(hab), -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint SQL bind error");
	return -1;
    }
    r = sqlite3_bind_text(insert_datapoint_stmt, param++, bionet_hab_get_id(hab), -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint SQL bind error");
	return -1;
    }
    r = sqlite3_bind_text(insert_datapoint_stmt, param++, bionet_node_get_id(node), -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint SQL bind error");
	return -1;
    }
    r = sqlite3_bind_text(insert_datapoint_stmt, param++, bionet_resource_get_id(resource), -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint SQL bind error");
	return -1;
    }
    r = sqlite3_bind_text(insert_datapoint_stmt, param++, bdm_id, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint SQL bind error");
	return -1;
    }

    while(SQLITE_BUSY == (r = sqlite3_step(insert_datapoint_stmt)));
    if (r != SQLITE_DONE) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint SQL error: %s\n", sqlite3_errmsg(db));
	sqlite3_reset(insert_datapoint_stmt);
        return -1;
    }
    sqlite3_reset(insert_datapoint_stmt);
    sqlite3_clear_bindings(insert_datapoint_stmt);

    return 0;
}




//
// this is the true database interface for the bdm front-end to use, and for the bdm back-end to provide
//

int db_add_datapoint(bionet_datapoint_t *datapoint) {
    int r;
    char *zErrMsg = NULL;
    bionet_value_t * value = NULL;
    bionet_resource_t * resource = NULL;
    bionet_node_t * node = NULL;
    bionet_hab_t * hab = NULL;

    // start transaction
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


    // add parent objects as needed
    value = bionet_datapoint_get_value(datapoint);
    resource = bionet_value_get_resource(value);
    node = bionet_resource_get_node(resource);
    hab = bionet_node_get_hab(node);

    r = add_hab_to_db(hab);
    if (r != 0) goto fail;

    r = add_node_to_db(node);
    if (r != 0) goto fail;

    r = add_resource_to_db(resource);
    if (r != 0) goto fail;

    //BDM-BP TODO: Add BDM to database if not self

    // now finally add the data point itself
    r = add_datapoint_to_db(datapoint);
    if (r != 0) goto fail;


    // it all worked, commit it to the DB
    r = do_commit();
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
    uint8_t resource_key[BDM_RESOURCE_KEY_LENGTH],
    const char * bdm_id,
    struct timeval *timestamp,
    bionet_resource_data_type_t type,
    void * value)
{
    int r;

    // Single insert, so no transaction needed

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
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint-sync SQL error: %s\n", sqlite3_errmsg(db));
	    return -1;
	}
    }

    double entry = gettimedouble();

    // Bind host variables to the prepared statement 
    // -- This eliminates the need to escape strings
    // Bind in order of the placeholders (?) in the SQL
    int param = 1;
    r = sqlite3_bind_blob(insert_resource_stmt, param++, 
        resource_key, BDM_RESOURCE_KEY_LENGTH, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint SQL bind error");
	return -1;
    }

    int ival;
    double  dval;
    switch(type) {
        case BIONET_RESOURCE_DATA_TYPE_BINARY:
            ival = *(int*)value;
            r = sqlite3_bind_int(insert_datapoint_sync_stmt, param++, ival);
            break;
            
        case BIONET_RESOURCE_DATA_TYPE_UINT8:
            ival = *(uint8_t*)value;
            r = sqlite3_bind_int(insert_datapoint_sync_stmt, param++, ival);
            
        case BIONET_RESOURCE_DATA_TYPE_INT8:
            ival = *(int8_t*)value;
            r = sqlite3_bind_int(insert_datapoint_sync_stmt, param++, ival);
            break;
            
        case BIONET_RESOURCE_DATA_TYPE_UINT16:
            ival = *(uint16_t*)value;
            r = sqlite3_bind_int(insert_datapoint_sync_stmt, param++, ival);
            break;
            
        case BIONET_RESOURCE_DATA_TYPE_INT16:
            ival = *(int16_t*)value;
            r = sqlite3_bind_int(insert_datapoint_sync_stmt, param++, ival);
            break;
            
        case BIONET_RESOURCE_DATA_TYPE_UINT32:
            ival = *(uint32_t*)value;
            r = sqlite3_bind_int(insert_datapoint_sync_stmt, param++, ival);
            break;
            
        case BIONET_RESOURCE_DATA_TYPE_INT32:
            ival = *(int32_t*)value;
            r = sqlite3_bind_int(insert_datapoint_sync_stmt, param++, ival);
            break;
            
        case BIONET_RESOURCE_DATA_TYPE_FLOAT:
            dval = *(float*)value;
            r = sqlite3_bind_double(insert_datapoint_sync_stmt, param++, dval);
            break;
            
        case BIONET_RESOURCE_DATA_TYPE_DOUBLE:
            dval = *(float*)value;
            r = sqlite3_bind_double(insert_datapoint_sync_stmt, param++, dval);
            break;
            
        case BIONET_RESOURCE_DATA_TYPE_STRING:
            r = sqlite3_bind_text(insert_datapoint_sync_stmt, param++, value, -1, SQLITE_STATIC);
            break;
        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint API Type error");
            return -1;
    }
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint SQL bind error");
	return -1;
    }

    r = sqlite3_bind_int( insert_datapoint_sync_stmt, param++,  timestamp->tv_sec);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint SQL bind error");
	return -1;
    }

    r = sqlite3_bind_int( insert_datapoint_sync_stmt, param++,  timestamp->tv_usec);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint SQL bind error");
	return -1;
    }

    r = sqlite3_bind_double( insert_datapoint_sync_stmt, param++,  entry);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint SQL bind error");
	return -1;
    }

    r = sqlite3_bind_text(insert_datapoint_sync_stmt, param++, bdm_id, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint SQL bind error");
	return -1;
    }

    while(SQLITE_BUSY == (r = sqlite3_step(insert_datapoint_sync_stmt)));
    if (r != SQLITE_DONE) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint SQL error: %s\n", sqlite3_errmsg(db));
	sqlite3_reset(insert_datapoint_sync_stmt);
        return -1;
    }
    sqlite3_reset(insert_datapoint_sync_stmt);
    sqlite3_clear_bindings(insert_datapoint_sync_stmt);

    return 0;
}


int db_add_node(bionet_node_t *node) {
    int i;

    int r;
    char *zErrMsg = NULL;


    // start transaction
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


    // add parent hab
    r = add_hab_to_db(bionet_node_get_hab(node));
    if (r != 0) goto fail;


    // add this node
    r = add_node_to_db(node);
    if (r != 0) goto fail;


    // add this node's resources
    for (i = 0; i < bionet_node_get_num_resources(node); i++) {
        bionet_resource_t *resource = bionet_node_get_resource_by_index(node, i);
        bionet_datapoint_t *d = bionet_resource_get_datapoint_by_index(resource, 0);

        r = add_resource_to_db(resource);
        if (r != 0) goto fail;

        // add the resource's data point, if any
        if (d != NULL) {
	    // BDM-BP TODO: Add BDM to database if not self
            r = add_datapoint_to_db(d);
            if (r != 0) goto fail;
        }
    }


    // it all worked, commit it to the DB
    r = do_commit();
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




int db_add_hab(bionet_hab_t *hab) {
    int r;

    // this doesnt need a transaction because it's just a single INSERT
    r = add_hab_to_db(hab);
    if (r != 0) {
        return -1;
    }

    return 0;
}




static bionet_hab_t *find_hab(GPtrArray *hab_list, const char *hab_type, const char *hab_id) {
    int i;
    bionet_hab_t *hab;

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


static int db_get_resource_datapoints_callback(
    void *sql_ret_void,
    int argc,
    char **argv,
    char **azColName
) {
    sql_return_t * sql_ret_val = (sql_return_t *)sql_ret_void;
    GPtrArray *hab_list = sql_ret_val->hab_list;
    struct timeval * latest_entry = sql_ret_val->latest_entry;
    bionet_hab_t *hab;
    bionet_node_t *node;
    bionet_resource_t *resource;

    struct timeval timestamp, entry;
    int err = 0;
    bionet_value_t * value = NULL;
    bionet_datapoint_t * datapoint = NULL;
    char * tmpstr;

    hab = find_hab(hab_list, argv[0], argv[1]);
    if (hab == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "db_get_resource_datapoints_callback(): error finding hab %s.%s", argv[0], argv[1]);
        return -1;
    }

    node = find_node(hab, argv[2]);
    if (node == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
	      "db_get_resource_datapoints_callback(): error finding node %s.%s.%s", 
	      bionet_hab_get_type(hab), bionet_hab_get_id(hab), argv[2]);
        return -1;
    }

    resource = find_resource(node, argv[3], argv[4], argv[5]);
    if (resource == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, 
	      "db_get_resource_datapoints_callback(): error finding resource %s %s %s.%s.%s:%s", 
	      argv[3], argv[4], 
	      bionet_hab_get_type(hab), bionet_hab_get_id(hab), 
	      bionet_node_get_id(node), argv[5]);
        return -1;
    }

    timestamp.tv_sec = atoi(argv[7]);
    timestamp.tv_usec = atoi(argv[8]);

    doubletotimeval(strtod(argv[9], NULL), &entry);
    if ((entry.tv_sec > latest_entry->tv_sec) ||
	(entry.tv_sec == latest_entry->tv_sec && entry.tv_usec > latest_entry->tv_usec)) {
	latest_entry->tv_sec = entry.tv_sec;
	latest_entry->tv_usec = entry.tv_usec;
    }

    switch(bionet_resource_get_data_type(resource))
    {
    case BIONET_RESOURCE_DATA_TYPE_BINARY:
	value = bionet_value_new_binary(resource, atoi(argv[6]));
	break; 
    case BIONET_RESOURCE_DATA_TYPE_UINT8:
	value = bionet_value_new_uint8(resource, (uint8_t)strtoul(argv[6], NULL, 0));
	break; 
    case BIONET_RESOURCE_DATA_TYPE_INT8:   
	value = bionet_value_new_int8(resource, (int8_t)atoi(argv[6]));
	break; 
    case BIONET_RESOURCE_DATA_TYPE_UINT16: 
	value = bionet_value_new_uint16(resource, (uint16_t)strtoul(argv[6], NULL, 0));
	break; 
    case BIONET_RESOURCE_DATA_TYPE_INT16:  
	value = bionet_value_new_int16(resource, (int16_t)atoi(argv[6]));
	break; 
    case BIONET_RESOURCE_DATA_TYPE_UINT32: 
	value = bionet_value_new_uint32(resource, (uint32_t)strtoul(argv[6], NULL, 0));
	break; 
    case BIONET_RESOURCE_DATA_TYPE_INT32:  
	value = bionet_value_new_int32(resource, (int32_t)atoi(argv[6]));
	break; 
    case BIONET_RESOURCE_DATA_TYPE_FLOAT:  
	value = bionet_value_new_float(resource, strtof(argv[6], NULL));
	break; 
    case BIONET_RESOURCE_DATA_TYPE_DOUBLE: 
	value = bionet_value_new_double(resource, strtod(argv[6], NULL));
	break; 
    case BIONET_RESOURCE_DATA_TYPE_STRING:
	tmpstr = malloc(strlen(argv[6]) + 1);
	strncpy(tmpstr, argv[6], strlen(argv[6]) + 1);
	value = bionet_value_new_str(resource, tmpstr);
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


GPtrArray *db_get_resource_datapoints(
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const char *resource_id,
    struct timeval *datapoint_start,
    struct timeval *datapoint_end,
    struct timeval *entry_start,
    struct timeval *entry_end,
    struct timeval *latest_entry
) {
    int r;
    char sql[2048];
    char *zErrMsg = NULL;

    char hab_type_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    char hab_id_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    char node_id_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    char resource_id_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    char datapoint_start_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    char datapoint_end_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    /* The size of these is mostly fixed */
    char entry_start_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    char entry_end_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];

    GPtrArray *hab_list;

    hab_list = g_ptr_array_new();

    sql_return_t sql_ret_val = { hab_list, 
				 latest_entry };

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


    if ((datapoint_start == NULL) ||
	((datapoint_start->tv_sec == 0) && (datapoint_start->tv_usec == 0))) {
	datapoint_start_restriction[0] = '\0';
    } else {
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

    if ((datapoint_end == NULL) ||
	((datapoint_end->tv_sec == 0) && (datapoint_end->tv_usec == 0))) {
	datapoint_end_restriction[0] = '\0';
    } else {
	r = snprintf(
		datapoint_end_restriction, 
		sizeof(datapoint_end_restriction),
		"AND ("
		" Datapoints.Timestamp_Sec < %d"
		" OR (Datapoints.Timestamp_Sec = %d AND Datapoints.Timestamp_Usec < %d)"
		")",
		(int)datapoint_end->tv_sec, 
		(int)datapoint_end->tv_sec, 
		(int)datapoint_end->tv_usec);
	if (r >= sizeof(datapoint_end_restriction)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "db_get_resource_datapoints(): datapoint end time is too long!");
	    return NULL;
	}
    }


    if ((entry_start == NULL) ||
	((entry_start->tv_sec == 0) && (entry_start->tv_usec == 0))) {
	entry_start_restriction[0] = '\0';
    } else {
	r = snprintf(entry_start_restriction, sizeof(entry_start_restriction),
		"AND Datapoints.Entry_Timestamp >= %lf",
		timevaltodouble(entry_start));
	if (r >= sizeof(entry_start_restriction)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "db_get_resource_entrys(): entry start time is too long!");
	    return NULL;
	}
    }

    if ((entry_end == NULL) ||
	((entry_end->tv_sec == 0) && (entry_end->tv_usec == 0))) {
	entry_end_restriction[0] = '\0';
    } else {
	r = snprintf(entry_end_restriction, sizeof(entry_end_restriction),
		"AND Datapoints.Entry_Timestamp < %lf",
		timevaltodouble(entry_end));
	if (r >= sizeof(entry_end_restriction)) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "db_get_resource_entrys(): entry end time is too long!");
	    return NULL;
	}
    }

    r = snprintf(
        sql,
        sizeof(sql),
        "SELECT"
        "    Hardware_Abstractors.HAB_TYPE,"
        "    Hardware_Abstractors.HAB_ID,"
        "    Nodes.Node_ID,"
        "    Resource_Data_Types.Data_Type,"
        "    Resource_Flavors.Flavor,"
        "    Resources.Resource_ID,"
        "    Datapoints.Value,"
        "    Datapoints.Timestamp_Sec,"
        "    Datapoints.Timestamp_Usec,"
	"    Datapoints.Entry_Timestamp"
        " FROM"
        "    Hardware_Abstractors,"
        "    Nodes,"
        "    Resources,"
        "    Resource_Data_Types,"
        "    Resource_Flavors,"
        "    Datapoints"
        " WHERE"
        "    Nodes.HAB_Key=Hardware_Abstractors.Key"
        "    AND Resources.Node_Key=Nodes.Key"
        "    AND Resource_Data_Types.Key=Resources.Data_Type_Key"
        "    AND Resource_Flavors.Key=Resources.Flavor_Key"
        "    AND Datapoints.Resource_Key=Resources.Key"
	"    %s"
	"    %s"
	"    %s"
	"    %s"
	"    %s"
	"    %s"
	"    %s"
	"    %s"
	" ORDER BY"
	"     Datapoints.Timestamp_Sec ASC,"
	"     Datapoints.Timestamp_Usec ASC"
	";",
	datapoint_start_restriction,
	datapoint_end_restriction,
	entry_start_restriction,
	entry_end_restriction,
	hab_type_restriction,
	hab_id_restriction,
	node_id_restriction,
	resource_id_restriction
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


    r = sqlite3_exec(
        db,
        sql,
        db_get_resource_datapoints_callback,
        &sql_ret_val,
        &zErrMsg
    );

    if (r == SQLITE_BUSY) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "failed to get datapoints because the database is busy (\"%s\")", zErrMsg);
        sqlite3_free(zErrMsg);
        return NULL;
    }

    if (r != SQLITE_OK) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "db_get_resource_datapoints(): SQL error: %s", zErrMsg);
        sqlite3_free(zErrMsg);
        return NULL;
    }


    return hab_list;
}


GPtrArray *db_get_metadata(
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const char *resource_id,
    struct timeval *datapoint_start,
    struct timeval *datapoint_end,
    struct timeval *entry_start,
    struct timeval *entry_end,
    struct timeval *latest_entry) {

    //TODO complete this function
    
    return NULL;
} 


void db_get_latest_entry_timestamp(struct timeval * ts) {
    //TODO complete this function
}


void db_get_last_sync_timestamp(char * bdm_id, struct timeval * ts) {
    //TODO complete this function
}


void db_set_last_sync_timestamp(char * bdm_id, struct timeval * ts) {
    //TODO complete this function
}


// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
