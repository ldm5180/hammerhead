
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sqlite3.h>
#include <openssl/sha.h>

#include <glib.h>

#include <bionet.h>

#include "bdm-util.h"




#define DB_NAME "bdm.db"

static sqlite3 *db = NULL;




int db_init(void) {
    int r;

    r = sqlite3_open(DB_NAME, &db);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error opening database %s: %s\n", DB_NAME, sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    r = sqlite3_busy_timeout(db, 5 * 1000);
    if (r != 0) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error setting busy timeout on database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    return 0;
}


// 
// shut down the database
//

void db_shutdown(void) {
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




// 
// database back-end internal helper functions
//
// Each of these functions does a single SQL INSERT.
// Returns 0 on success, -1 on failure.
//


static int add_hab_to_db(const bionet_hab_t *hab) {
    int r;
    char *zErrMsg = NULL;
    char sql[1024];


    r = snprintf(
        sql,
        sizeof(sql),
        "INSERT"
        " OR IGNORE"
        " INTO Hardware_Abstractors"
        " VALUES ( NULL, '%s', '%s' )"
        ";",
        hab->type,
        hab->id
    );
    if (r >= sizeof(sql)) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-hab SQL doesnt fit in buffer!\n");
        return -1;
    }

    r = sqlite3_exec(
        db,
        sql,
        NULL,
        0,
        &zErrMsg
    );
    if (r != SQLITE_OK) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "insert-hab SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }

    return 0;
}




static int add_node_to_db(const bionet_node_t *node) {
    int r;
    char sql[1024];
    char *zErrMsg = NULL;


    r = snprintf(
        sql,
        sizeof(sql),
        "INSERT"
        " OR IGNORE"
        " INTO Nodes"
        " SELECT"
        "     NULL, Hardware_Abstractors.Key, '%s'"
        "     FROM Hardware_Abstractors"
        "     WHERE"
        "         HAB_Type='%s'"
        "         AND HAB_ID='%s'"
        ";",
        node->id,
        node->hab->type,
        node->hab->id
    );
    if (r >= sizeof(sql)) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-node SQL doesnt fit in buffer!\n");
        return -1;
    }

    r = sqlite3_exec(
        db,
        sql,
        NULL,
        0,
        &zErrMsg
    );
    if (r != SQLITE_OK) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "insert-node SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }

    return 0;
}




static int add_resource_to_db(bionet_resource_t *resource) {
    int r;
    char sql[1024];
    char *zErrMsg = NULL;

    SHA_CTX sha_ctx;
    uint8_t byte;
    unsigned char sha_digest[SHA_DIGEST_LENGTH];

    // blob is "X'" followed by 64 bits (8 bytes) of data ASCII-encoded as
    // 16 characters in [0-9a-f], followed by "'" and the terminating NULL
    char blob[20];


    r = SHA1_Init(&sha_ctx);
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error initializing SHA1 context\n");
        return -1;
    }

    r = SHA1_Update(&sha_ctx, resource->node->hab->type, strlen(resource->node->hab->type));
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource HAB-Type\n");
        return -1;
    }

    r = SHA1_Update(&sha_ctx, resource->node->hab->id, strlen(resource->node->hab->id));
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource HAB-ID\n");
        return -1;
    }

    r = SHA1_Update(&sha_ctx, resource->node->id, strlen(resource->node->id));
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource Node-ID\n");
        return -1;
    }

    r = SHA1_Update(&sha_ctx, resource->id, strlen(resource->id));
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource ID\n");
        return -1;
    }

    byte = resource->data_type;
    r = SHA1_Update(&sha_ctx, &byte, 1);
    if (r != 1) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "error updating SHA1 context with Resource Data Type\n");
        return -1;
    }

    byte = resource->flavor;
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


    // the first 64 bits of the SHA digest is the GURID blob
    // FIXME: or just use the whole thing?
    {
        int i;

        sprintf(blob, "X'");
        for (i = 0; i < 8; i ++) {
            sprintf(&blob[2+(2*i)], "%02X", sha_digest[i]);
        }
        sprintf(&blob[2+(2*8)], "'");
    }


    r = snprintf(
        sql,
        sizeof(sql),
        "INSERT"
        " OR IGNORE"
        " INTO Resources"
        " SELECT"
        "     %s, Nodes.Key, '%s', Resource_Data_Types.Key, Resource_Flavors.Key"
        "     FROM Hardware_Abstractors, Nodes, Resource_Data_Types, Resource_Flavors"
        "     WHERE"
        "         Hardware_Abstractors.HAB_Type='%s'"
        "         AND Hardware_Abstractors.HAB_ID='%s'"
        "         AND Nodes.HAB_Key=Hardware_Abstractors.Key"
        "         AND Nodes.Node_ID='%s'"
        "         AND Resource_Data_Types.Data_Type LIKE '%s'"
        "         AND Resource_Flavors.Flavor LIKE '%s'"
        ";",
        blob,
        resource->id,
        resource->node->hab->type,
        resource->node->hab->id,
        resource->node->id,
        bionet_resource_data_type_to_string(resource->data_type),
        bionet_resource_flavor_to_string(resource->flavor)
    );
    if (r >= sizeof(sql)) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-resource SQL doesnt fit in buffer!\n");
        return -1;
    }

    r = sqlite3_exec(
        db,
        sql,
        NULL,
        0,
        &zErrMsg
    );
    if (r != SQLITE_OK) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "insert-resource SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }

    return 0;
}




static int add_datapoint_to_db(bionet_datapoint_t *datapoint) {
    int r;
    char sql[1024];
    char escaped_string[1024];
    char *zErrMsg = NULL;

    // String values need to be escaped here to avoid an SQL injection
    // vulnerability.  Single-quote characters "'" need to be replaced with
    // the two-character string "''" (two single-quote characters).

    {
        const char *src_string;
        int src_index;
        int dest_index;
        int dest_size;

        src_string = bionet_datapoint_value_to_string(datapoint);

        dest_size = sizeof(escaped_string);

        for (
            src_index = 0, dest_index = 0;
            (dest_index < (dest_size-1)) && (src_string[src_index] != '\0');
            src_index ++, dest_index ++
        ) {
            if (src_string[src_index] == '\'') {
                if (dest_index >= (dest_size-2)) {
                    // not enough room for the escaped ' and the NULL
                    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint SQL doesnt fit in buffer!\n");
                    return -1;
                }
                escaped_string[dest_index] = '\'';
                dest_index ++;
            }
            escaped_string[dest_index] = src_string[src_index];
        }
        escaped_string[dest_index] = '\0';
    }


    // FIXME: might be the wrong resource (might differ in data type or flavor)
    r = snprintf(
        sql,
        sizeof(sql),
        "INSERT"
        " OR IGNORE"
        " INTO Datapoints"
        " SELECT"
        "     NULL, Resources.Key, '%s', %u, %u"
        "     FROM Hardware_Abstractors, Nodes, Resources"
        "     WHERE"
        "         Hardware_Abstractors.HAB_Type = '%s'"
        "         AND Hardware_Abstractors.HAB_ID = '%s'"
        "         AND Nodes.HAB_Key = Hardware_Abstractors.Key"
        "         AND Nodes.Node_ID = '%s'"
        "         AND Resources.Node_Key = Nodes.Key"
        "         AND Resources.Resource_ID = '%s'"
        ";",
        escaped_string,
        (unsigned int)datapoint->timestamp.tv_sec,
        (unsigned int)datapoint->timestamp.tv_usec,
        datapoint->resource->node->hab->type,
        datapoint->resource->node->hab->id,
        datapoint->resource->node->id,
        datapoint->resource->id
    );
    if (r >= sizeof(sql)) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint SQL doesnt fit in buffer!\n");
        return -1;
    }

    r = sqlite3_exec(
        db,
        sql,
        NULL,
        0,
        &zErrMsg
    );
    if (r != SQLITE_OK) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "add-datapoint SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }

    return 0;
}




//
// this is the true database interface for the bdm front-end to use, and for the bdm back-end to provide
//

int db_add_datapoint(bionet_datapoint_t *datapoint) {
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


    // add parent objects as needed
    r = add_hab_to_db(datapoint->resource->node->hab);
    if (r != 0) goto fail;

    r = add_node_to_db(datapoint->resource->node);
    if (r != 0) goto fail;

    r = add_resource_to_db(datapoint->resource);
    if (r != 0) goto fail;


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




int db_add_node(bionet_node_t *node) {
    GSList *i;

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
    r = add_hab_to_db(node->hab);
    if (r != 0) goto fail;


    // add this node
    r = add_node_to_db(node);
    if (r != 0) goto fail;


    // add this node's resources
    for (i = node->resources; i != NULL; i = i->next) {
        bionet_resource_t *resource = i->data;

        r = add_resource_to_db(resource);
        if (r != 0) goto fail;
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
        if ((strcmp(hab->type, hab_type) == 0) && (strcmp(hab->id, hab_id) == 0)) {
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

    bionet_hab_add_node(hab, node);

    return node;
}


static bionet_resource_t *find_resource(bionet_node_t *node, const char *data_type, const char *flavor, const char *resource_id) {
    bionet_resource_t *resource;

    resource = bionet_node_get_resource_by_id(node, resource_id);
    if (resource != NULL) return resource;

    resource = bionet_resource_new_from_str(node, data_type, flavor, resource_id);
    if (resource == NULL) {
        return NULL;
    }

    bionet_node_add_resource(node, resource);

    return resource;
}


static int db_get_resource_datapoints_callback(
    void *hab_list_void,
    int argc,
    char **argv,
    char **azColName
) {
    GPtrArray *hab_list = hab_list_void;
    bionet_hab_t *hab;
    bionet_node_t *node;
    bionet_resource_t *resource;

    struct timeval timestamp;


    hab = find_hab(hab_list, argv[0], argv[1]);
    if (hab == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "db_get_resource_datapoints_callback(): error finding hab %s.%s", argv[0], argv[1]);
        return -1;
    }

    node = find_node(hab, argv[2]);
    if (node == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "db_get_resource_datapoints_callback(): error finding node %s.%s.%s", hab->type, hab->id, argv[2]);
        return -1;
    }

    resource = find_resource(node, argv[3], argv[4], argv[5]);
    if (resource == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "db_get_resource_datapoints_callback(): error finding resource %s %s %s.%s.%s:%s", argv[3], argv[4], hab->type, hab->id, node->id, argv[5]);
        return -1;
    }

    timestamp.tv_sec = atoi(argv[7]);
    timestamp.tv_usec = atoi(argv[8]);

    if (bionet_resource_add_datapoint(resource, argv[6], &timestamp) == NULL) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "db_get_resource_datapoints_callback(): error adding datapoint");
        return -1;
    }


    return 0;
}


GPtrArray *db_get_resource_datapoints(
    const char *hab_type,
    const char *hab_id,
    const char *node_id,
    const char *resource_id,
    struct timeval *start,
    struct timeval *end
) {
    int r;
    char sql[2048];
    char *zErrMsg = NULL;

    char hab_type_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    char hab_id_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    char node_id_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];
    char resource_id_restriction[2 * BIONET_NAME_COMPONENT_MAX_LEN];

    GPtrArray *hab_list;

    hab_list = g_ptr_array_new();


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
        "    Datapoints.Timestamp_Usec"
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
        "    AND ("
        "        (Datapoints.Timestamp_Sec = %d AND Datapoints.Timestamp_Usec >= %d)"
        "        OR (Datapoints.Timestamp_Sec > %d AND Datapoints.Timestamp_Sec < %d)"
        "        OR (Datapoints.Timestamp_Sec = %d AND Datapoints.Timestamp_Usec <= %d)"
        "    )"
        "    %s"
        "    %s"
        "    %s"
        "    %s"
        " ORDER BY"
        "     Datapoints.Timestamp_Sec ASC,"
        "     Datapoints.Timestamp_Usec ASC"
        ";",
        (int)start->tv_sec,
        (int)start->tv_usec,
        (int)start->tv_sec,
        (int)end->tv_sec,
        (int)end->tv_sec,
        (int)end->tv_usec,
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
        hab_list,
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


