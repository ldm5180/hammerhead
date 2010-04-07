
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef WINDOWS
    #include <winsock2.h>
#endif

#include <glib.h>
#include <sqlite3.h>

#include <libutil.h>
#include <bionet.h>




#define DEFAULT_DB_NAME "bdm.db"

static sqlite3 *db = NULL;

bionet_hab_t **habs = NULL;
int num_habs = 0;
int hab_array_size = 0;




static int db_resource_value_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    char time_str[100];
    struct tm *tm;
    time_t t;
    int r;

    t = atoi(argv[1]);
    tm = gmtime(&t);
    if (tm == NULL) {
        fprintf(stderr, "error with gmtime: %s", strerror(errno));
        return 0;
    }

    r = strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm);
    if (r <= 0) {
        fprintf(stderr, "error with strftime: %s", strerror(errno));
        return 0;
    }

    printf("            %s @ %s.%06d\n", argv[0], time_str, atoi(argv[2]));
    return 0;
}




static void db_handle_resource_row(sqlite3_stmt *statement, bionet_node_t *node) {
    bionet_resource_t *resource;

    const char *resource_id;

    int blob_len;
    const uint8_t *blob;
    int i;
    char fingerprint[17];  // 16 nibbles (8 bytes, 64 bits) plus NULL


    resource_id = (const char *)sqlite3_column_text(statement, 2);
    fprintf(
        stderr,
        "fetched a Resource %s.%s.%s:%s\n",
        node->hab_type,
        node->hab_id,
        node->id,
        resource_id
    );

    blob = sqlite3_column_blob(statement, 0);
    blob_len = sqlite3_column_bytes(statement, 0);
    if (blob_len != 8) {
        g_error("resource key length is %d, expected 8\n", blob_len);
        return;
    }
    for (i = 0; i < 8; i ++) {
        sprintf(&fingerprint[i*2], "%02X", blob[i]);
    }


    resource = bionet_resource_new(
        node->hab_type,
        node->hab_id,
        node->id,
        BIONET_RESOURCE_DATA_TYPE_MIN,  // probably not the real data type
        BIONET_RESOURCE_FLAVOR_MIN,     // probably not the real flavor
        resource_id
    );
    if (resource == NULL) {
        g_warning("error making resource %s.%s.%s:%s (resource id was %s)\n", node->hab_type, node->hab_id, node->id, fingerprint, resource_id);
        return;
    }

    resource->user_data = strdup(fingerprint);
    if (resource->user_data == NULL) {
        g_warning("error duping fingerprint\n");
        bionet_resource_free(resource);
        return;
    }

    bionet_node_add_resource(node, resource);
}




static int db_node_callback(void *hab_void, int argc, char **argv, char **azColName){
    bionet_hab_t *hab = hab_void;
    bionet_node_t *node;

    int r;
    char sql[1024];
    sqlite3_stmt *statement;

    node = bionet_node_new(hab->type, hab->id, argv[2]);
    if (node == NULL) {
        g_warning("error making node %s.%s.%s\n", hab->type, hab->id, argv[2]);
        return -1;
    }
    node->user_data = strdup(argv[0]);
    if (node->user_data == NULL) {
        g_warning("out of memory making node %s.%s.%s\n", hab->type, hab->id, argv[2]);
        return -1;
    }
    bionet_hab_add_node(hab, node);

    fprintf(stderr, "fetched a Node %s.%s.%s\n", hab->type, hab->id, argv[2]);


    // 
    // fetch this Node's Resources
    //

    r = snprintf(
        sql,
        sizeof(sql),
        "SELECT * FROM Resources WHERE Node_Key='%s'",
        argv[0]
    );
    if (r >= sizeof(sql)) {
        g_warning("select-nodes SQL doesnt fit in buffer!\n");
        return -1;  // FIXME: ok?
    }

    r = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,      // sqlite3_stmt **ppStmt,  /* OUT: Statement handle */
        NULL             // const char **pzTail     /* OUT: Pointer to unused portion of zSql */
    );
    if (r != SQLITE_OK) {
        g_warning("error preparing SQL statement [%s]: %s\n", sql, sqlite3_errmsg(db));
        return -1;
    }

    do {
        r = sqlite3_step(statement);
        if (r == SQLITE_DONE) break;
        if (r != SQLITE_ROW) {
            g_warning("sqlite3_step() error: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(statement);
            return -1;
        }
        db_handle_resource_row(statement, node);
    } while (r == SQLITE_ROW);

    r = sqlite3_finalize(statement);
    if (r != SQLITE_OK) {
        g_warning("sqlite3_finalize() error: %s\n", sqlite3_errmsg(db));
        return -1;
    }


    return 0;
}




static int db_hab_callback(void *NotUsed, int argc, char **argv, char **azColName){
    int r;
    char *zErrMsg = NULL;
    char sql[1024];


    // 
    // add this HAB to our list of HABs
    //

    if ((num_habs + 1) > hab_array_size) {
        if (hab_array_size == 0) hab_array_size = 10;
        else hab_array_size *= 2;
        habs = realloc(habs, hab_array_size * sizeof(bionet_hab_t *));
        if (habs == NULL) {
            g_warning("out of memory!\n");
            return -1;
        }
    }

    habs[num_habs] = bionet_hab_new(argv[1], argv[2]);
    num_habs ++;

    fprintf(stderr, "fetched a HAB %s.%s\n", argv[1], argv[2]);


    // 
    // fetch this HAB's Nodes
    //

    r = snprintf(
        sql,
        sizeof(sql),
        "SELECT * FROM Nodes WHERE HAB_Key='%s'",
        argv[0]
    );
    if (r >= sizeof(sql)) {
        g_warning("select-nodes SQL doesnt fit in buffer!\n");
        return -1;  // FIXME: ok?
    }

    r = sqlite3_exec(
        db,
        sql,
        db_node_callback,
        habs[num_habs - 1],
        &zErrMsg
    );
    if (r != SQLITE_OK) {
        g_warning("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    return 0;
}




void print_resource(bionet_resource_t *resource) {
    int r;
    char *zErrMsg = NULL;
    char sql[1024];

    struct timeval start, end;
    int show_time = 1;

    printf(
        "        %s %s %s:\n",
        bionet_resource_data_type_to_string(resource->data_type),
        bionet_resource_flavor_to_string(resource->flavor),
        resource->id
    );

    r = gettimeofday(&start, NULL);
    if (r != 0) {
        g_warning("error with gettimeofday: %s\n", strerror(errno));
        show_time = 0;
    }

    r = snprintf(
        sql,
        sizeof(sql),
        "SELECT"
        " Resource_Values.Value, Resource_Values.Timestamp_Sec, Resource_Values.Timestamp_Usec"
        " FROM Resource_Values"
        " WHERE"
        "     Resource_Values.Resource_Key=X'%s'"
        " ORDER BY"
        "     Resource_Values.Timestamp_Sec ASC,"
        "     Resource_Values.Timestamp_Usec ASC"
        ";",
        (char *)resource->user_data
    );
    if (r >= sizeof(sql)) {
        g_warning("select-resource-values SQL doesnt fit in buffer!\n");
        return;
    }

    r = sqlite3_exec(
        db,
        sql,
        db_resource_value_callback,
        0,
        &zErrMsg
    );
    if (r != SQLITE_OK) {
        g_warning("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    r = gettimeofday(&end, NULL);
    if (r != 0) {
        g_warning("error with gettimeofday: %s\n", strerror(errno));
        show_time = 0;
    }

    if (show_time) {
        double t = tv_diff_double_seconds(&start, &end);
        fprintf(stderr, "fetching all datapoints for the Resource took %.06f seconds\n", t);
    }

    // g_usleep(100 * 1000);
}




void print_node(bionet_node_t *node) {
    int num_resources = bionet_node_num_resources(node);
    int i;

    printf("   %s:\n", node->id);

    for (i = 0; i < num_resources; i ++) {
        bionet_resource_t *resource;

        resource = bionet_node_get_resource_by_index(node, i);
        if (resource == NULL) {
            g_error("unexpected NULL Resource while iterating over %s.%s.%s Resources\n", node->hab_type, node->hab_id, node->id);
            exit(1);
        }

        print_resource(resource);
    }
}




void print_hab(bionet_hab_t *hab) {
    int num_nodes = bionet_hab_num_nodes(hab);
    int i;

    printf("%s.%s:\n", hab->type, hab->id);

    for (i = 0; i < num_nodes; i ++) {
        bionet_node_t *node;

        node = bionet_hab_get_node_by_index(hab, i);
        if (node == NULL) {
            g_error("unexpected NULL Node when iterating over %s.%s Nodes\n", hab->type, hab->id);
            exit(1);
        }

        print_node(node);
    }
}





int main(int argc, char *argv[]) {
    int i;
    int r;
    char *zErrMsg = NULL;

    char *db_name;


    if (argv[1] != NULL) {
        db_name = argv[1];
    } else {
        db_name = DEFAULT_DB_NAME;
    }


    // 
    // set up sqlite3
    //

    r = sqlite3_open(db_name, &db);
    if (r != 0) {
        g_error("error opening database %s: %s\n", db_name, sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    r = sqlite3_busy_timeout(db, 5 * 1000);
    if (r != 0) {
        g_error("error setting busy timeout on database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }


    // 
    // fetch all the HABs, Nodes, and Resources from the database
    //

    {
        struct timeval start, end;
        int show_time = 1;

        r = gettimeofday(&start, NULL);
        if (r != 0) {
            g_warning("error with gettimeofday: %s\n", strerror(errno));
            show_time = 0;
        }

        r = sqlite3_exec(
            db,
            "SELECT * FROM Hardware_Abstractors;",
            db_hab_callback,
            0,
            &zErrMsg
        );
        if (r != SQLITE_OK) {
            g_warning("SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }

        r = gettimeofday(&end, NULL);
        if (r != 0) {
            g_warning("error with gettimeofday: %s\n", strerror(errno));
            show_time = 0;
        }

        if (show_time) {
            double t = tv_diff_double_seconds(&start, &end);
            fprintf(stderr, "fetching all HABs, Nodes, and resources took %.06f seconds\n", t);
        }
    }



    // 
    // iterate over all the HABs & their Nodes and for each fetch the Resources & Datapoints
    //

    for (i = 0; i < num_habs; i ++) {
        bionet_hab_t *hab = habs[i];

        if (hab == NULL) {
            g_error("unexpected NULL HAB while iterating over HABs\n");
            exit(1);
        }

        print_hab(hab);
    }


    //
    // done
    //

    sqlite3_close(db);
    return 0;
}

