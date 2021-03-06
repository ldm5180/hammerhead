#include "check-bdm.h"
#include "bionet-data-manager.h"
#include "bdm-db.h"

#include <unistd.h>
#include "util/protected.h"

static sqlite3 * db = NULL;

static const struct timeval ts_50_0 = {50, 0};
static const struct timeval ts_50_9 = {50, 900000};
static const struct timeval ts_51_0 = {51, 0};
static const struct timeval ts_51_1 = {51, 100000};
static const struct timeval ts_51_2 = {51, 200000};
static const struct timeval ts_51_3 = {51, 300000};
static const struct timeval ts_52_1 = {52, 100000};
static const struct timeval ts_55_0 = {55, 0};
static const struct timeval ts_60_0 = {60, 0};


/*
 * Helper functions used by several tests in the file.
 *
 */
static int timeval_cmp(const struct timeval * a, const struct timeval *b) {
    int r;
    r = b->tv_sec - a->tv_sec;
    if(r) return r;


    r = b->tv_usec - a->tv_usec;

    return r;
}

static void insert_test_bdms(sqlite3 *db) {
    int r;
    sqlite_int64 rowid;

    r = db_insert_bdm(db, "bdm-id", &rowid);
    fail_unless(r == 0, "Failed to insert BDM");
}


static void insert_test_habs(sqlite3 *db) {
    int r;
    sqlite_int64 rowid;

    insert_test_bdms(db);

    r = db_insert_hab(db, "hab-type", "hab-id", &rowid);
    fail_unless(r == 0, "Failed to insert hab");
}


static void insert_test_nodes(sqlite3 *db, 
        uint8_t guid1[BDM_RESOURCE_KEY_LENGTH],
        uint8_t guid2[BDM_RESOURCE_KEY_LENGTH]) {
    int r;
    sqlite_int64 rowid;

    insert_test_habs(db);

    bionet_hab_t * hab = bionet_hab_new("hab-type", "hab-id");

    bionet_node_t *node = bionet_node_new(hab, "node-id");
    bionet_resource_t *resource = bionet_resource_new(node, 
            BIONET_RESOURCE_DATA_TYPE_STRING, 
            BIONET_RESOURCE_FLAVOR_SENSOR,
            "resource-id");
    bionet_node_add_resource(node, resource);

    r = db_make_node_guid(node, guid1);
    fail_unless(r == 0, "Failed to create guid");

    r = db_insert_node(db, "node-id", "hab-type", "hab-id", guid1, &rowid);
    fail_unless(r == 0, "Failed to insert node");




    // Second node with same ID, but different resources
    node = bionet_node_new(hab, "node-id");
    resource = bionet_resource_new(node, 
            BIONET_RESOURCE_DATA_TYPE_STRING, 
            BIONET_RESOURCE_FLAVOR_SENSOR,
            "resource-id");
    bionet_node_add_resource(node, resource);

    resource = bionet_resource_new(node, 
            BIONET_RESOURCE_DATA_TYPE_STRING, 
            BIONET_RESOURCE_FLAVOR_SENSOR,
            "resource-id-2");
    bionet_node_add_resource(node, resource);


    r = db_make_node_guid(node, guid2);
    fail_unless(r == 0, "Failed to create guid");

    r = db_insert_node(db, "node-id", "hab-type", "hab-id", guid2, &rowid);
    fail_unless(r == 0, "Failed to insert node");
}

/*
static void insert_test_resources(sqlite3 *db) {
    int r;
    sqlite_int64 rowid;

    uint8_t guid1[BDM_RESOURCE_KEY_LENGTH];
    uint8_t guid2[BDM_RESOURCE_KEY_LENGTH];
    insert_test_nodes(db, guid1, guid2);

    r = db_insert_resource(db, "hab-type", "hab-id", "node-id", guid, "resource-id",
            BIONET_RESOURCE_FLAVOR_SENSOR,
            BIONET_RESOURCE_DATA_TYPE_STRING,
            &rowid);
    fail_unless(r == 0, "Failed to insert resource");

}
*/


/* Callback for db_get_events */
bdm_handle_row_status_t count_events(
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
        void * usr_data)
{
    int * pNum = (int*)usr_data;

    (*pNum)++;

    return BDM_HANDLE_OK;
}

// Verify that the datapoint matches all these parameters.
//
// NOTE: Currently, this assumes that there is exactly one element at every level
void check_hab_list_for_dp_str(
        GPtrArray * hab_list,
        const char * bdm_id,
        const char * hab_type,
        const char * hab_id,
        const char * node_id, 
        const char * resource_id,
        const struct timeval *dp_ts,
        const struct timeval *event_ts,
        const char * val_str,
        int dp_idx)
{

    bionet_hab_t * hab = g_ptr_array_index(hab_list, 0);
    fail_if(NULL == hab, "No value for hab %d", 0);
    fail_if(strcmp(hab_id, bionet_hab_get_id(hab)), "Wrong id for hab");
    fail_if(strcmp(hab_type, bionet_hab_get_type(hab)), "Wrong type for hab");

    fail_unless(1 == bionet_hab_get_num_nodes(hab),
            "Wrong number of nodes returned (%d, not %d)", 
            bionet_hab_get_num_nodes(hab), 1);

    bionet_node_t * node = bionet_hab_get_node_by_index(hab, 0);
    fail_if(NULL == node, "No value for node %d", 0);
    fail_if(strcmp(node_id, bionet_node_get_id(node)), "Wrong node-id");

    fail_unless(1 == bionet_node_get_num_resources(node),
            "Wrong number of resources for node");
    bionet_resource_t * resource = bionet_node_get_resource_by_index(node, 0);
    fail_if(NULL == resource, "Failed to get resource from node");
    fail_if(strcmp(resource_id, bionet_resource_get_id(resource)), 
            "Wrong resource-id");


    char * str;
    bionet_datapoint_t * datapoint;
    bionet_event_t * event;
    
    datapoint = bionet_resource_get_datapoint_by_index(resource, dp_idx);
    if(NULL == val_str) {
        fail_unless(NULL == datapoint, "Datapoint %d found when none expected", dp_idx);
    } else {
        fail_if(NULL == datapoint, "Failed to get datapoint pointer");

        str = bionet_resource_value_to_string(resource, dp_idx);
        fail_if(strcmp(str, val_str), 
                    "Wrong string value for datapoint: %s", str);

        if(dp_ts) {
            struct timeval *ts = bionet_datapoint_get_timestamp(datapoint);
            fail_unless(0 == timeval_cmp(ts, dp_ts), 
                    "Wrong time for datapoint");
        }

        event = bionet_datapoint_get_event_by_index(datapoint, 0);
        fail_if(NULL == event, "No event from database");
        if(event_ts){
            fail_unless(0 == timeval_cmp(event_ts, bionet_event_get_timestamp(event)), "Wrong time for event");
        }

        fail_unless(0 == strcmp(bdm_id, bionet_event_get_bdm_id(event)), "Wrong bdm id for event");
    }
}


START_TEST (check_db_schema)
{
    int r;
    sqlite_int64 rowid;

    sqlite3 * db = db_init(":memory:");

    fail_if(db == NULL, "DB is NULL");

    r = db_insert_bdm(db, "TestBDM", &rowid);
    fail_unless(r == 0, "Failed to insert BDM");
    fail_unless(rowid == 1, "First inserted BDM has rowid other than 1");

    db_shutdown(db);

}
END_TEST


void db_insert_setup() {
#if 1
    db = db_init(":memory:");
#else
    unlink("/tmp/check.db");
    db = db_init("/tmp/check.db");
#endif

    fail_if(db == NULL, "DB is NULL");
}

void db_insert_teardown() {
    db_shutdown(db);
}

START_TEST (check_db_insert_bdm)
{
    int r;
    sqlite_int64 rowid;

    r = db_insert_bdm(db, "bdm", &rowid);
    fail_unless(r == 0, "Failed to insert BDM");
    fail_unless(rowid == 1, "Got wrong rowid for first bdm");

    r = db_insert_bdm(db, "bdm", &rowid);
    fail_unless(r > 0, "Failed to insert BDM");
    fail_unless(rowid == 1, "Got wrong rowid when re-inserting first bdm");

    r = db_insert_bdm(db, "bdm2", &rowid);
    fail_unless(r == 0, "Failed to insert BDM");
    fail_unless(rowid == 2, "Got wrong rowid for second bdm");

    r = db_insert_bdm(db, "bdm", &rowid);
    fail_unless(r > 0, "Failed to insert BDM");
    fail_unless(rowid == 1, "Got wrong rowid when re-inserting first bdm after the second");

}
END_TEST


START_TEST (check_db_insert_hab)
{
    int r;
    sqlite_int64 rowid;

    insert_test_bdms(db);

    r = db_insert_hab(db, "hab-type", "hab-id", &rowid);
    fail_unless(r == 0, "Failed to insert HAB");
    fail_unless(rowid == 1, "Got wrong rowid for first hab");

    r = db_insert_hab(db, "hab-type", "hab-id", &rowid);
    fail_unless(r > 0, "Failed to insert HAB");
    fail_unless(rowid == 1, "Got wrong rowid when re-inserting first hab");

    r = db_insert_hab(db, "hab-type-2", "hab-id-2", &rowid);
    fail_unless(r == 0, "Failed to insert HAB 2");
    fail_unless(rowid == 2, "Got wrong rowid for second hab");

    r = db_insert_hab(db, "hab-type", "hab-id", &rowid);
    fail_unless(r > 0, "Failed to insert HAB");
    fail_unless(rowid == 1, "Got wrong rowid when re-inserting first hab after the second");

}
END_TEST

START_TEST (check_db_insert_node)
{
    int r;
    sqlite_int64 rowid;
    uint8_t guid1[BDM_RESOURCE_KEY_LENGTH];
    uint8_t guid2[BDM_RESOURCE_KEY_LENGTH];

    insert_test_habs(db);

    bionet_hab_t * hab = bionet_hab_new("hab-type", "hab-id");
    {
        bionet_node_t *node = bionet_node_new(hab, "node-id");
        bionet_resource_t *resource = bionet_resource_new(node, 
                BIONET_RESOURCE_DATA_TYPE_STRING, 
                BIONET_RESOURCE_FLAVOR_SENSOR,
                "resource-id");
        bionet_node_add_resource(node, resource);

        r = db_make_node_guid(node, guid1);
        fail_unless(r == 0, "Failed to create guid");
    }

    {
        bionet_node_t *node = bionet_node_new(hab, "node-id-2");
        bionet_resource_t *resource = bionet_resource_new(node, 
                BIONET_RESOURCE_DATA_TYPE_STRING, 
                BIONET_RESOURCE_FLAVOR_SENSOR,
                "resource-id");
        bionet_node_add_resource(node, resource);

        r = db_make_node_guid(node, guid2);
        fail_unless(r == 0, "Failed to create guid");
    }


    r = db_insert_node(db, "node-id", "hab-type", "hab-id", guid1, &rowid);
    fail_unless(r == 0, "Failed to insert Node");
    fail_unless(rowid == 1, "Got wrong rowid for first node");

    r = db_insert_node(db, "node-id", "hab-type", "hab-id", guid1, &rowid);
    fail_unless(r > 0, "Failed to insert Node");
    fail_unless(rowid == 1, "Got wrong rowid when re-inserting first node");

    r = db_insert_node(db, "node-id-2", "hab-type", "hab-id", guid2, &rowid);
    fail_unless(r == 0, "Failed to insert Node");
    fail_unless(rowid == 2, "Got wrong rowid for second node: %d", rowid);

    r = db_insert_node(db, "node-id", "hab-type", "hab-id", guid1, &rowid);
    fail_unless(r > 0, "Failed to insert Node");
    fail_unless(rowid == 1, "Got wrong rowid when re-inserting first node after the second");

}
END_TEST

START_TEST (check_db_insert_resource)
{
    int r;
    uint8_t guid1[BDM_RESOURCE_KEY_LENGTH];
    uint8_t guid2[BDM_RESOURCE_KEY_LENGTH];
    insert_test_nodes(db, guid1, guid2);

    r = db_insert_resource(db, "hab-type", "hab-id", "node-id", guid2, "resource-id",
            BIONET_RESOURCE_FLAVOR_SENSOR,
            BIONET_RESOURCE_DATA_TYPE_STRING);
    fail_unless(r == 0, "Failed to insert first Resource");

    r = db_insert_resource(db, "hab-type", "hab-id", "node-id", guid2, "resource-id",
            BIONET_RESOURCE_FLAVOR_SENSOR,
            BIONET_RESOURCE_DATA_TYPE_STRING);
    fail_unless(r > 0, "Failed to insert duplicate  Resource");

    r = db_insert_resource(db, "hab-type", "hab-id", "node-id", guid2, "resource-id-2",
            BIONET_RESOURCE_FLAVOR_SENSOR,
            BIONET_RESOURCE_DATA_TYPE_STRING);
    fail_unless(r == 0, "Failed to insert second Resource");

    r = db_insert_resource(db, "hab-type", "hab-id", "node-id", guid2, "resource-id",
            BIONET_RESOURCE_FLAVOR_SENSOR,
            BIONET_RESOURCE_DATA_TYPE_STRING);
    fail_unless(r > 0, "Failed to insert duplicate Resource");

}
END_TEST


START_TEST (check_db_insert_hab_event)
{
    int r;
    sqlite_int64 bdmrow;
    sqlite_int64 habrow;
    sqlite_int64 eventrow;


    r = db_insert_bdm(db, "bdm-id", &bdmrow);
    fail_unless(r == 0, "Failed to insert BDM");

    r = db_insert_hab(db, "hab-type", "hab-id", &habrow);
    fail_unless(r == 0, "Failed to insert hab");

    r = db_insert_event(db, &ts_51_2, bdmrow, DBB_NEW_HAB_EVENT, habrow, &eventrow);
    fail_unless(r == 0, "Failed to insert HAB event");

    int seq = db_get_latest_entry_seq(db);
    fail_unless(seq == eventrow, "The last known seq (%d) isn't the last event (%d)",
            seq, eventrow);

    {
        GPtrArray * hab_list;
        bionet_event_t * event;

        // Event timestamp filter matches nothing
        {
            hab_list = db_get_habs(db,
                    "bdm-id", "hab-type", "hab-id", &ts_51_3, &ts_52_1, -1, -1);

            fail_if(NULL == hab_list, "Error running query");
            fail_unless(hab_list->len == 0, "Results returned when no rows should match");
        }

        // Filters match all timestamps
        {
            hab_list = db_get_habs(db,
                    "bdm-id", "hab-type", "hab-id", &ts_50_9, &ts_51_3, -1, -1);

            fail_if(NULL == hab_list, "Error running query");
        }


        fail_unless(hab_list->len == 1, 
                "Wrong number of HABs returned (%d, not %d)", 
                hab_list->len, 1);

        bionet_hab_t * hab = g_ptr_array_index(hab_list, 0);
        fail_if(NULL == hab, "No value for hab %d", 0);
        fail_if(strcmp("hab-id", bionet_hab_get_id(hab)), "Wrong id for hab");
        fail_if(strcmp("hab-type", bionet_hab_get_type(hab)), "Wrong type for hab");

        event = bionet_hab_get_event_by_index(hab, 0);
        fail_if(NULL == event, "No event from database");
        fail_unless(0 == timeval_cmp(&ts_51_2, bionet_event_get_timestamp(event)), "Wrong time for event");
        fail_unless(0 == strcmp("bdm-id", bionet_event_get_bdm_id(event)), "Wrong bdm id for event");


    }


    db_shutdown(db);

}
END_TEST

static void _insert_node(
        sqlite_int64 bdmrow,
        sqlite_int64 habrow,
        sqlite_int64 * node_eventrow,
        sqlite_int64 * dp_eventrow,
        uint8_t guid[BDM_RESOURCE_KEY_LENGTH]
        )
{
    sqlite_int64 noderow;
    sqlite_int64 dprow;
    int r;

    bionet_hab_t * hab = bionet_hab_new("hab-type", "hab-id");
    bionet_node_t *node = bionet_node_new(hab, "node-id");
    bionet_resource_t *resource = bionet_resource_new(node, 
            BIONET_RESOURCE_DATA_TYPE_STRING, 
            BIONET_RESOURCE_FLAVOR_SENSOR,
            "node-id");
    fail_if(NULL == resource, "Error creating resource");

    uint8_t guid_local[BDM_RESOURCE_KEY_LENGTH];
    if(guid == NULL) {
        guid = guid_local;
    }
    r = db_make_node_guid(node, guid);
    fail_unless(r == 0, "Failed to create guid");

    r = db_insert_node(db, "node-id", "hab-type", "hab-id", guid, &noderow);
    fail_unless(r == 0, "Failed to insert node");


    r = db_insert_resource(db, "hab-type", "hab-id", "node-id", guid, "resource-id",
            BIONET_RESOURCE_FLAVOR_SENSOR,
            BIONET_RESOURCE_DATA_TYPE_STRING);
    fail_unless(r == 0, "Failed to insert resource");


    r = db_insert_event(db, &ts_51_2, bdmrow, DBB_NEW_NODE_EVENT, noderow, node_eventrow);
    fail_unless(r == 0, "Failed to insert node event");

    if(dp_eventrow) {
        db_make_resource_key("hab-type", "hab-id", "node-id", "resource-id",
                BIONET_RESOURCE_DATA_TYPE_STRING,
                BIONET_RESOURCE_FLAVOR_SENSOR,
                guid);

        bdm_datapoint_t dp = {{ 0 }};
        dp.timestamp.tv_sec = 50;
        dp.timestamp.tv_usec = 5000;
        dp.type = DB_STRING;
        dp.value.str = "value string";
        dp.rowid = 0;

        r = db_insert_datapoint(db, guid, &dp, &dprow);
        fail_unless(r == 0, "Failed to insert datapoint");


        r = db_insert_event(db, &ts_51_2, bdmrow, DBB_DATAPOINT_EVENT, dprow, dp_eventrow);
        fail_unless(r == 0, "Failed to insert BDM");
    }
}

static void _insert_datapoints(void){
    int r;
    sqlite_int64 bdmrow;
    sqlite_int64 habrow;
    sqlite_int64 dprow;


    r = db_insert_bdm(db, "bdm-id", &bdmrow);
    fail_unless(r == 0, "Failed to insert BDM");

    r = db_insert_hab(db, "hab-type", "hab-id", &habrow);
    fail_unless(r == 0, "Failed to insert hab");

    r = db_insert_event(db, &ts_51_2, bdmrow, DBB_NEW_HAB_EVENT, habrow, NULL);
    fail_unless(r == 0, "Failed to insert HAB event");


    // Insert datapoints
    int sec;
    uint8_t guid[BDM_RESOURCE_KEY_LENGTH];
    _insert_node(bdmrow, habrow, NULL, NULL, guid);
    for(sec=50; sec<55; sec++) {
        char str_val[255];
        db_make_resource_key("hab-type", "hab-id", "node-id", "resource-id",
                BIONET_RESOURCE_DATA_TYPE_STRING,
                BIONET_RESOURCE_FLAVOR_SENSOR,
                guid);

        bdm_datapoint_t dp = {{ 0 }};
        dp.timestamp.tv_sec = sec;
        dp.timestamp.tv_usec = 5000;
        dp.type = DB_STRING;

        snprintf(str_val, sizeof(str_val), "value string %d", sec);
        dp.value.str = strdup(str_val);
        dp.rowid = 0;

        r = db_insert_datapoint(db, guid, &dp, &dprow);
        fail_unless(r == 0, "Failed to insert datapoint");


        r = db_insert_event(db, &ts_51_2, bdmrow, DBB_DATAPOINT_EVENT, dprow, NULL);
        fail_unless(r == 0, "Failed to insert BDM");
    }
}

START_TEST (check_db_insert_node_event)
{
    int r;
    sqlite_int64 bdmrow;
    sqlite_int64 habrow;
    sqlite_int64 eventrow;


    r = db_insert_bdm(db, "bdm-id", &bdmrow);
    fail_unless(r == 0, "Failed to insert BDM");

    r = db_insert_hab(db, "hab-type", "hab-id", &habrow);
    fail_unless(r == 0, "Failed to insert hab");


    _insert_node(bdmrow, habrow, &eventrow, NULL, NULL);


    int seq = db_get_latest_entry_seq(db);
    fail_unless(seq == eventrow, "The last known seq isn't whats reported");

    {
        GPtrArray * hab_list;
        bionet_event_t * event;

        // timestamp filter matches nothing
        {
            hab_list = db_get_nodes(db,
                    "bdm-id", "hab-type", "hab-id", "node-id", 
                    &ts_51_0, &ts_51_1, -1, -1);

            fail_if(NULL == hab_list, "Error running query");
            fail_unless(hab_list->len == 0, "Results returned when no rows should match");
        }

        // Filters match all timestamps
        {

            hab_list = db_get_nodes(db,
                    "bdm-id", "hab-type", "hab-id", "node-id", 
                    &ts_51_1, &ts_60_0, -1, -1);

            fail_if(NULL == hab_list, "Error running query");
        }


        fail_unless(hab_list->len == 1, 
                "Wrong number of HABs returned (%d, not %d)", 
                hab_list->len, 1);

        bionet_hab_t * hab = g_ptr_array_index(hab_list, 0);
        fail_if(NULL == hab, "No value for hab %d", 0);
        fail_if(strcmp("hab-id", bionet_hab_get_id(hab)), "Wrong id for hab");
        fail_if(strcmp("hab-type", bionet_hab_get_type(hab)), "Wrong type for hab");

        fail_unless(1 == bionet_hab_get_num_nodes(hab),
                "Wrong number of nodes returned (%d, not %d)", 
                bionet_hab_get_num_nodes(hab), 1);

        bionet_node_t * node = bionet_hab_get_node_by_index(hab, 0);
        fail_if(NULL == node, "No value for node %d", 0);
        fail_if(strcmp("node-id", bionet_node_get_id(node)), "Wrong node-id");

        event = bionet_node_get_event_by_index(node, 0);
        fail_if(NULL == event, "No event from database");
        fail_unless(0 == timeval_cmp(&ts_51_2, bionet_event_get_timestamp(event)), "Wrong time for event");
        fail_unless(0 == strcmp("bdm-id", bionet_event_get_bdm_id(event)), "Wrong bdm id for event");

        fail_unless(1 == bionet_node_get_num_resources(node),
                "Wrong number of resources (%d) for node", 
                bionet_node_get_num_resources(node));
        bionet_resource_t * resource = bionet_node_get_resource_by_index(node, 0);
        fail_if(NULL == resource, "Failed to get resource from node");
        fail_if(strcmp("resource-id", bionet_resource_get_id(resource)), 
                "Wrong resource-id");
    }


    db_shutdown(db);

}
END_TEST


START_TEST (check_db_insert_datapoint_event)
{
    int r;
    sqlite_int64 bdmrow;
    sqlite_int64 habrow;
    sqlite_int64 eventrow;


    r = db_insert_bdm(db, "bdm-id", &bdmrow);
    fail_unless(r == 0, "Failed to insert BDM");

    r = db_insert_hab(db, "hab-type", "hab-id", &habrow);
    fail_unless(r == 0, "Failed to insert hab");

    r = db_insert_event(db, &ts_51_2, bdmrow, DBB_NEW_HAB_EVENT, habrow, NULL);
    fail_unless(r == 0, "Failed to insert HAB event");


    _insert_node(bdmrow, habrow, NULL, &eventrow, NULL);


    int seq = db_get_latest_entry_seq(db);
    fail_unless(seq == eventrow, "The last known seq isn't whats reported");

    {
        GPtrArray * hab_list;
        bionet_event_t * event;

        // Datapoint timestamp filter matches nothing
        {
            struct timeval dpstart = {51,0};
            struct timeval dpend = {51,5000};
            int num_events = 0;

            // Verify that db_get_events returns no events
            r = db_get_events(db,
                    "bdm-id", "hab-type", "hab-id", "node-id", "resource-id",
                    &dpstart, &dpend, &ts_51_0, &ts_51_3, -1, -1,
                    _DB_GET_DP_EVENTS, count_events, (void*)&num_events);

            fail_if(r != 0, "Error running query");
            fail_unless(num_events == 0, "Results returned when no rows should match");
        }

        // Filters match all timestamps
        {
            struct timeval dpstart = {50,0};
            struct timeval dpend = {51,0};

            hab_list = db_get_resource_datapoints(db,
                    "bdm-id", "hab-type", "hab-id", "node-id", "resource-id",
                    &dpstart, &dpend, &ts_51_0, &ts_51_3, -1, -1);

            fail_if(NULL == hab_list, "Error running query");
        }


        fail_unless(hab_list->len == 1, 
                "Wrong number of HABs returned (%d, not %d)", 
                hab_list->len, 1);

        bionet_hab_t * hab = g_ptr_array_index(hab_list, 0);
        fail_if(NULL == hab, "No value for hab %d", 0);
        fail_if(strcmp("hab-id", bionet_hab_get_id(hab)), "Wrong id for hab");
        fail_if(strcmp("hab-type", bionet_hab_get_type(hab)), "Wrong type for hab");

        fail_unless(1 == bionet_hab_get_num_nodes(hab),
                "Wrong number of nodes returned (%d, not %d)", 
                bionet_hab_get_num_nodes(hab), 1);

        bionet_node_t * node = bionet_hab_get_node_by_index(hab, 0);
        fail_if(NULL == node, "No value for node %d", 0);
        fail_if(strcmp("node-id", bionet_node_get_id(node)), "Wrong node-id");

        fail_unless(1 == bionet_node_get_num_resources(node),
                "Wrong number of resources for node");
        bionet_resource_t * resource = bionet_node_get_resource_by_index(node, 0);
        fail_if(NULL == resource, "Failed to get resource from node");
        fail_if(strcmp("resource-id", bionet_resource_get_id(resource)), 
                "Wrong resource-id");


        char * str;
        struct timeval tv;
        r = bionet_resource_get_str(resource, &str, &tv);
        fail_unless(0 == r, "Failed to get string resource");
        fail_if(strcmp("value string", str), "Wrong string value for datapoint");
        fail_unless(tv.tv_sec == 50 && tv.tv_usec == 5000,
                "Wrong timestamp for datapoint");

        bionet_datapoint_t * datapoint = bionet_resource_get_datapoint_by_index(resource, 0);
        fail_if(NULL == datapoint, "Failed to get datapoint pointer");

        event = bionet_datapoint_get_event_by_index(datapoint, 0);
        fail_if(NULL == event, "No event from database");
        fail_unless(0 == timeval_cmp(&ts_51_2, bionet_event_get_timestamp(event)), "Wrong time for event");
        fail_unless(0 == strcmp("bdm-id", bionet_event_get_bdm_id(event)), "Wrong bdm id for event");


    }

    db_shutdown(db);
}
END_TEST

START_TEST (check_db_insert_all_events)
{
    int r;
    sqlite_int64 bdmrow;
    sqlite_int64 habrow;
    sqlite_int64 eventrow;


    r = db_insert_bdm(db, "bdm-id", &bdmrow);
    fail_unless(r == 0, "Failed to insert BDM");

    r = db_insert_hab(db, "hab-type", "hab-id", &habrow);
    fail_unless(r == 0, "Failed to insert hab");

    r = db_insert_event(db, &ts_51_2, bdmrow, DBB_NEW_HAB_EVENT, habrow, NULL);
    fail_unless(r == 0, "Failed to insert HAB event");

    _insert_node(bdmrow, habrow, NULL, &eventrow, NULL);

    int seq = db_get_latest_entry_seq(db);
    fail_unless(seq == eventrow, "The last known seq isn't whats reported");

    {
        GPtrArray * hab_list;
        bionet_event_t * event;

        // Datapoint timestamp filter matches nothing
        {
            hab_list = db_get_metadata(db,
                    "bdm-id", "hab-type", "hab-id", "node-id", "resource-id",
                    &ts_50_0, &ts_51_1, -1, -1);

            fail_if(NULL == hab_list, "Error running query");
            fail_unless(hab_list->len == 0, "Results returned when no rows should match");

        }

        // Filters match all timestamps
        {

            hab_list = db_get_metadata(db,
                    "bdm-id", "hab-type", "hab-id", "node-id", "resource-id",
                    &ts_50_0, &ts_55_0, -1, -1);

            fail_if(NULL == hab_list, "Error running query");
        }


        fail_unless(hab_list->len == 1, 
                "Wrong number of HABs returned (%d, not %d)", 
                hab_list->len, 1);

        bionet_hab_t * hab = g_ptr_array_index(hab_list, 0);
        fail_if(NULL == hab, "No value for hab %d", 0);
        fail_if(strcmp("hab-id", bionet_hab_get_id(hab)), "Wrong id for hab");
        fail_if(strcmp("hab-type", bionet_hab_get_type(hab)), "Wrong type for hab");

        event = bionet_hab_get_event_by_index(hab, 0);
        fail_if(NULL == event, "No event from database");
        fail_unless(0 == timeval_cmp(&ts_51_2, bionet_event_get_timestamp(event)), "Wrong time for event");
        fail_unless(0 == strcmp("bdm-id", bionet_event_get_bdm_id(event)), "Wrong bdm id for event");


        fail_unless(1 == bionet_hab_get_num_nodes(hab),
                "Wrong number of nodes returned (%d, not %d)", 
                bionet_hab_get_num_nodes(hab), 1);

        bionet_node_t * node = bionet_hab_get_node_by_index(hab, 0);
        fail_if(NULL == node, "No value for node %d", 0);
        fail_if(strcmp("node-id", bionet_node_get_id(node)), "Wrong node-id");

        event = bionet_node_get_event_by_index(node, 0);
        fail_if(NULL == event, "No event from database");
        fail_unless(0 == timeval_cmp(&ts_51_2, bionet_event_get_timestamp(event)), "Wrong time for event");
        fail_unless(0 == strcmp("bdm-id", bionet_event_get_bdm_id(event)), "Wrong bdm id for event");

        fail_unless(1 == bionet_node_get_num_resources(node),
                "Wrong number of resources for node");
        bionet_resource_t * resource = bionet_node_get_resource_by_index(node, 0);
        fail_if(NULL == resource, "Failed to get resource from node");
        fail_if(strcmp("resource-id", bionet_resource_get_id(resource)), 
                "Wrong resource-id");

    }


    db_shutdown(db);

}
END_TEST

START_TEST (check_db_get_most_recent)
{
    _insert_datapoints();

    {
        GPtrArray * hab_list;

        // Datapoint timestamp filter matches nothing,
        // but should return the newest datapoint
        {
            struct timeval dpstart = {56,0};
            struct timeval dpend = {57,5000};

            hab_list = db_get_resource_datapoints(db,
                    "bdm-id", "hab-type", "hab-id", "node-id", "resource-id",
                    &dpstart, &dpend, NULL, NULL, -1, -1);

            fail_if(NULL == hab_list, "Error running query");
        }

        fail_unless(hab_list->len == 1, 
                "Wrong number of HABs returned (%d, not %d)", 
                hab_list->len, 1);

        struct timeval dp_ts = {54,5000};
        check_hab_list_for_dp_str(hab_list, 
                    "bdm-id", "hab-type", "hab-id", "node-id", "resource-id",
                    &dp_ts, NULL, "value string 54", 0);
    }

    db_shutdown(db);
}
END_TEST

/*
 * Request the datapoints with timestamps between a and b, 
 * which matches no datapoints, but there are datapoints both before and 
 * after this range
 *
 * EXPECT: The newest datapoint before a should be returned
 */
START_TEST (check_db_get_most_recent_with_end_filter)
{
    _insert_datapoints();

    {
        GPtrArray * hab_list;

        // Datapoint timestamp filter matches nothing,
        // but should return the newest datapoint
        {
            struct timeval dpstart = {52,1000};
            struct timeval dpend = {52,2000};

            hab_list = db_get_resource_datapoints(db,
                    "bdm-id", "hab-type", "hab-id", "node-id", "resource-id",
                    &dpstart, &dpend, NULL, NULL, -1, -1);

            fail_if(NULL == hab_list, "Error running query");
        }

        fail_unless(hab_list->len == 1, 
                "Wrong number of HABs returned (%d, not %d)", 
                hab_list->len, 1);

        struct timeval dp_ts = {51,5000};
        check_hab_list_for_dp_str(hab_list, 
                    "bdm-id", "hab-type", "hab-id", "node-id", "resource-id",
                    &dp_ts, NULL, "value string 51", 0);
    }

    db_shutdown(db);
}
END_TEST

/*
 * Request the datapoints with timestamps between a and b, 
 * which matches exactly 1 datapoint
 *
 * EXPECT: The matching datapoint is returned
 */
START_TEST (check_db_get_most_recent_with_matching_filter)
{
    _insert_datapoints();

    {
        GPtrArray * hab_list;

        // Datapoint timestamp filter matches nothing,
        // but should return the newest datapoint
        struct timeval dpstart = {54,1000};
        struct timeval dpend = {54,9000};

        hab_list = db_get_resource_datapoints(db,
                "bdm-id", "hab-type", "hab-id", "node-id", "resource-id",
                &dpstart, &dpend, NULL, NULL, -1, -1);

        fail_if(NULL == hab_list, "Error running query");

        fail_unless(hab_list->len == 1, 
                "Wrong number of HABs returned (%d, not %d)", 
                hab_list->len, 1);

        struct timeval dp_ts = {54,5000};
        check_hab_list_for_dp_str(hab_list, 
                    "bdm-id", "hab-type", "hab-id", "node-id", "resource-id",
                    &dp_ts, NULL, "value string 54", 0);
        check_hab_list_for_dp_str(hab_list, 
                    "bdm-id", "hab-type", "hab-id", "node-id", "resource-id",
                    &dp_ts, NULL, NULL, 1);

        int r;
        int num_events = 0;
        r = db_get_events(db,
                "bdm-id", "hab-type", "hab-id", "node-id", "resource-id",
                &dpstart, &dpend, NULL, NULL, -1, -1,
                _DB_GET_DP_EVENTS, count_events, (void*)&num_events);

        fail_unless(r == 0, "Failed to count events");
        fail_unless(num_events == 1, "Wrong number of events: %d", num_events);

    }

    db_shutdown(db);
}
END_TEST


void check_bdm_db_init(Suite *s) {
    TCase * tc = tcase_create ("db");

    tcase_add_test (tc, check_db_schema);
    suite_add_tcase(s, tc);

    tc = tcase_create ("db-insert");
    tcase_add_checked_fixture(tc, db_insert_setup, db_insert_teardown);
    tcase_add_test (tc, check_db_insert_bdm);
    tcase_add_test (tc, check_db_insert_hab);
    tcase_add_test (tc, check_db_insert_node);
    tcase_add_test (tc, check_db_insert_resource);

    tcase_add_test (tc, check_db_insert_hab_event);
    tcase_add_test (tc, check_db_insert_node_event);
    tcase_add_test (tc, check_db_insert_datapoint_event);

    tcase_add_test (tc, check_db_insert_all_events);

    tcase_add_test (tc, check_db_get_most_recent);
    tcase_add_test (tc, check_db_get_most_recent_with_end_filter);
    tcase_add_test (tc, check_db_get_most_recent_with_matching_filter);


    suite_add_tcase(s, tc);
}


