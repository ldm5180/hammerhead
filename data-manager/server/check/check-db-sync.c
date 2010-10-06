#include "check-bdm.h"
#include "bionet-data-manager.h"
#include "bdm-db.h"

#include <unistd.h>
#include "util/protected.h"

static sqlite3 * db = NULL;

/*
 * Helper functions used by several tests in the file.
 *
 */

static int check_db_set_int(
    void *sql_ret_void,
    int argc,
    char **argv,
    char **azColName)
{
    fail_unless(argc == 1, "Too many columns returned");
    fail_unless(argv[0] != NULL, "NULL argv0");

    if(argv[0]) {
        *(int*)sql_ret_void = atoi(argv[0]);
    }

    return 0;
}



static void db_sync_setup() {
#if 1
    db = db_init(":memory:");
#else
    unlink("/tmp/check.db");
    db = db_init("/tmp/check.db");
#endif

    fail_if(db == NULL, "DB is NULL");

}

sync_sender_config_t check_sync_config;
static void setup_with_recipient(void)
{
    db_sync_setup();

    sync_config_init_default(&check_sync_config);

    check_sync_config.db = db;
    check_sync_config.frequency = 60;
    check_sync_config.last_metadata_sync = 3;
    check_sync_config.last_datapoint_sync = 3;
    check_sync_config.sync_recipient = strdup("localhost");

    // Add check_sync_config the first time
    fail_unless(0 == db_sync_sender_setup(&check_sync_config),
            "db_sync_sender_setup returned error");

}

void db_sync_teardown() {
    db_shutdown(db);
}

START_TEST (check_db_sync_recipient)
{
    sync_sender_config_t cfgSet, cfg1, cfg2;
    sync_config_init_default(&cfgSet);

    cfgSet.db = db;
    cfgSet.frequency = 60;
    cfgSet.last_metadata_sync = 3;
    cfgSet.last_datapoint_sync = 3;
    cfgSet.sync_recipient = strdup("localhost");

    // Make cfg1 cfg2 duplicates of cgfSet, then change the unique stuff
    memcpy(&cfg1, &cfgSet, sizeof(sync_sender_config_t));
    memcpy(&cfg2, &cfgSet, sizeof(sync_sender_config_t));


    // cfg 1 is same as cfgSet, but with different state
    cfg1.last_metadata_sync = -1;
    cfg1.last_datapoint_sync = -1;


    // cfg 2 is a different config...
    cfg2.last_metadata_sync = 5;
    cfg2.last_datapoint_sync = 5;
    cfg2.sync_recipient = strdup("other_host");


    // Add cfgSet the first time
    fail_unless(0 == db_sync_sender_setup(&cfgSet),
            "db_sync_sender_setup returned error");

    // Setup cfg1: This should load the state from the database 
    fail_unless(0 == db_sync_sender_setup(&cfg1),
            "db_sync_sender_setup returned error");

    fail_unless(cfg1.last_metadata_sync == cfgSet.last_metadata_sync,
            "Didn't load state from db");
    fail_unless(cfg1.last_datapoint_sync == cfgSet.last_datapoint_sync,
            "Didn't load state from db");


    // Setup cfg2: This should NOT load, becuase its unique constraint is different
    fail_unless(0 == db_sync_sender_setup(&cfg2),
            "db_sync_sender_setup returned error");

    fail_unless(cfg2.last_metadata_sync == 5,
            "Loaded wrong state from DB; should have added new record");
    fail_unless(cfg2.last_datapoint_sync == 5,
            "Loaded wrong state from DB; should have added new record");

}
END_TEST

START_TEST (check_db_sync_last_seq)
{
    sync_sender_config_t tmp_cfg; // Used to check database state
    sync_config_init_default(&tmp_cfg);
    tmp_cfg.db = db;
    tmp_cfg.frequency = 60;
    tmp_cfg.last_metadata_sync = 0;
    tmp_cfg.last_datapoint_sync = 0;
    tmp_cfg.sync_recipient = strdup("localhost");

    // Update datapoint only
    db_update_sync_seq(&check_sync_config, 11, 1);

    // Setup tmp_cfg to see if the last_sync held...
    fail_unless(0 == db_sync_sender_setup(&tmp_cfg),
            "db_sync_sender_setup returned error");

    fail_unless(11 == tmp_cfg.last_datapoint_sync,
            "db_update_sync_seq didn't update the datapoint seq");
    fail_unless(3 == tmp_cfg.last_metadata_sync,
            "db_update_sync_seq modified the metadata seq");

    // Update metadata only
    db_update_sync_seq(&check_sync_config, 99, 0);

    // Setup tmp_cfg to see if the last_sync held...
    fail_unless(0 == db_sync_sender_setup(&tmp_cfg),
            "db_sync_sender_setup returned error");

    fail_unless(99 == tmp_cfg.last_metadata_sync,
            "db_update_sync_seq didn't update the metadata seq");
    fail_unless(11 == tmp_cfg.last_datapoint_sync,
            "db_update_sync_seq modified the datapoint seq");
}
END_TEST

START_TEST (check_db_sync_sent)
{
    sqlite_int64 rowid_get, rowid;


    // Add a sync sent, and make sure it can be retrieved
    rowid = db_insert_sync_sent(db, 1, 3, 0);
    fail_unless(rowid > 0,
            "db_insert_sync_sent returned error");

    rowid_get = db_get_sync_sent(db, 1, 3, 0);
    fail_unless(rowid_get > 0,
            "db_get_sync_sent returned error");
    fail_unless(rowid_get == rowid,
            "db_get_sync_sent returned wrong rowid");

    // Add a duplicate sync sent, and make sure the old rowid is retured
    rowid_get = db_insert_sync_sent(db, 1, 3, 0);
    fail_unless(rowid_get > 0,
            "db_insert_sync_sent returned error");
    fail_unless(rowid_get == rowid,
            "db_insert_sync_sent didn't return existing row");

}
END_TEST

START_TEST (check_db_sync_ackd)
{
    sqlite_int64 rowid;
    int ival = -1;
    int r;
    char * zErrMsg = NULL;


    // "Record" the sync
    rowid = db_record_sync(&check_sync_config, 1, 3, 0);
    fail_unless(rowid > 0,
            "db_record_sync returned error");

    rowid = db_record_sync(&check_sync_config, 1, 3, 1);
    fail_unless(rowid > 0,
            "db_record_sync returned error");

    // Get number of outstanding syncs
    r = sqlite3_exec(db,
        "SELECT count(*) as Num from SyncsOutstanding",
        check_db_set_int,
        &ival,
        &zErrMsg
    );
    fail_unless(ival == 2, "Wrong number of syncs recorded: %d", ival);

    r = sqlite3_exec(db,
        "SELECT count(*) as Num from SyncsSent",
        check_db_set_int,
        &ival,
        &zErrMsg
    );
    fail_unless(ival == 2, "Wrong number of syncs recorded: %d", ival);

    // "Record" the ack
    rowid = db_record_sync_ack(check_sync_config.db, check_sync_config.db_key, 1, 3, 0);
    fail_unless(rowid == 0,
            "db_record_sync_ack returned error");

    rowid = db_record_sync_ack(check_sync_config.db, check_sync_config.db_key, 1, 3, 1);
    fail_unless(rowid == 0,
            "db_record_sync_ack returned error");

    // There should be no outstanding bundles left
    r = sqlite3_exec(db,
        "SELECT count(*) as Num from SyncsOutstanding",
        check_db_set_int,
        &ival,
        &zErrMsg
    );
    fail_unless(ival == 0, "Wrong number of syncs recorded: %d", ival);

    r = sqlite3_exec(db,
        "SELECT count(*) as Num from SyncsSent",
        check_db_set_int,
        &ival,
        &zErrMsg
    );
    fail_unless(ival == 0, "Wrong number of syncs recorded: %d", ival);

}
END_TEST


void check_bdm_db_sync(Suite *s) {
    TCase * tc = tcase_create ("db-sync-setup");

    tcase_add_checked_fixture(tc, db_sync_setup, db_sync_teardown);
    tcase_add_test (tc, check_db_sync_recipient);
    suite_add_tcase(s, tc);

    tc = tcase_create ("db-sync");
    tcase_add_checked_fixture(tc, setup_with_recipient, db_sync_teardown);
    tcase_add_test (tc, check_db_sync_last_seq);
    tcase_add_test (tc, check_db_sync_sent);
    tcase_add_test (tc, check_db_sync_ackd);
    suite_add_tcase(s, tc);


}


