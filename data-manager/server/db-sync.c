
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sqlite3.h>
#include <sys/time.h>
#include <glib.h>

#include <bionet.h>

#include <inttypes.h>

#include "bionet-data-manager.h"
#include "bdm-db.h"
#include "../../util/protected.h"

#undef _DUMP_SQL

extern sqlite3_stmt * all_stmts[NUM_PREPARED_STMTS];




// bind the columns that make up the SyncRecipients 
// table's unique contraint
static int _bind_recipient_unique_columns(
        sqlite3_stmt * stmt,
        int * idx,
        sync_sender_config_t * sync_config)
{
    int r;

    // eid
    r = sqlite3_bind_text(stmt, (*idx)++, sync_config->sync_recipient, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "SyncRecipients SQL bind error");
        return -1;
    }

    // Filter
    r = sqlite3_bind_text(stmt, (*idx)++, sync_config->resource_name_pattern, -1, SQLITE_STATIC);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "SyncRecipients SQL bind error");
        return -1;
    }

    // Start Sec
    r = sqlite3_bind_int(stmt, (*idx)++, sync_config->start_time.tv_sec);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "SyncRecipients SQL bind error");
        return -1;
    }

    // Start USec
    r = sqlite3_bind_int(stmt, (*idx)++, sync_config->start_time.tv_usec);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "SyncRecipients SQL bind error");
        return -1;
    }

    // End Sec
    r = sqlite3_bind_int(stmt, (*idx)++, sync_config->end_time.tv_sec);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "SyncRecipients SQL bind error");
        return -1;
    }

    // End USec
    r = sqlite3_bind_int(stmt, (*idx)++, sync_config->end_time.tv_usec);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "SyncRecipients SQL bind error");
        return -1;
    }

    return 0;
}

// Get the key for this sync recipient
static sqlite_int64 db_get_sync_recipient(sync_sender_config_t * sync_config) {
    sqlite_int64 rowid = -1;
    int r;

    if(all_stmts[GET_SYNC_RECIPIENT_STMT]  == NULL) {
	r = sqlite3_prepare_v2(sync_config->db, 
	    "SELECT Key, last_sync "
            "FROM SyncRecipients "
            "WHERE eid=? "
            "  AND filter=? "
            "  AND start_sec=? "
            "  AND start_usec=? "
            "  AND end_sec=? "
            "  AND end_usec=? ",
	    -1, &all_stmts[GET_SYNC_RECIPIENT_STMT] , NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s SQL error: %s", __FUNCTION__, sqlite3_errmsg(sync_config->db));
	    return -1;
	}

    }
    sqlite3_stmt * this_stmt = all_stmts[GET_SYNC_RECIPIENT_STMT];

    int param = 1;
    r = _bind_recipient_unique_columns(this_stmt, &param, sync_config);
    if ( r ) return -1;

    // Only one row should every return.
    // If more, we'll take the last one, which is as good as any metric
    for(;;) {
        r = sqlite3_step(this_stmt);
        if(r == SQLITE_BUSY) {
            g_usleep(20 * 1000);
            continue;
        }
        if(r == SQLITE_ROW) {
            rowid = sqlite3_column_int64(this_stmt, 0); 
            sync_config->db_key = rowid;
            sync_config->last_entry_end_seq = sqlite3_column_int(this_stmt, 1); 
        } else {
            break;
        }
    }
    if (r != SQLITE_DONE && r != SQLITE_ROW) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            "%s(): SQL error: %s", 
            __FUNCTION__, sqlite3_errmsg(sync_config->db));
    }

    sqlite3_reset(this_stmt);
    sqlite3_clear_bindings(this_stmt);

    return rowid;
}

// Get the key for this sync recipient
static int db_insert_sync_recipient(sync_sender_config_t * sync_config) {
    sqlite_int64 rowid = -1;
    int r;

    if(all_stmts[INSERT_SYNC_RECIPIENT_STMT]  == NULL) {
	r = sqlite3_prepare_v2(sync_config->db, 
	    "INSERT INTO SyncRecipients (eid,filter,start_sec,start_usec,end_sec,end_usec,last_sync) "
            "VALUES (?,?,?,?,?,?,?) ",
	    -1, &all_stmts[INSERT_SYNC_RECIPIENT_STMT] , NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s SQL error: %s", __FUNCTION__, sqlite3_errmsg(sync_config->db));
	    return -1;
	}
    }
    sqlite3_stmt * this_stmt = all_stmts[INSERT_SYNC_RECIPIENT_STMT];

    int param = 1;
    r = _bind_recipient_unique_columns(this_stmt, &param, sync_config);
    if ( r ) return -1;

    // Bind last_sync
    r = sqlite3_bind_int64(this_stmt, param++, sync_config->last_entry_end_seq);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "SyncRecipients SQL bind error");
        return -1;
    }

    while(SQLITE_BUSY == (r = sqlite3_step(this_stmt))){
        g_usleep(20 * 1000);
    }
        
    switch (r) {
        case SQLITE_DONE:
            // Success!
            rowid = sqlite3_last_insert_rowid(sync_config->db);
            sync_config->db_key = rowid;
            break;

        case SQLITE_CONSTRAINT:
            // Row already exists. Not Cool! We should have queried it first...
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s: %s", 
                    __FUNCTION__, sqlite3_errmsg(sync_config->db));
            break;

        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s SQL error: %s",
                   __FUNCTION__, sqlite3_errmsg(sync_config->db));
            break;
    }

    sqlite3_reset(this_stmt);
    sqlite3_clear_bindings(this_stmt);

    return rowid;
}

// Lookup / Initialize the database for this sync sender
int db_sync_sender_setup(sync_sender_config_t * sync_config) {

    sqlite_int64 db_key;

    db_key = db_get_sync_recipient(sync_config);

    if(db_key < 0 ) {
        db_key = db_insert_sync_recipient(sync_config);
        if(db_key < 0 ) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "Unable to setup sync-recipient");
            return -1;
        }
    }

    return 0;
}


sqlite_int64 db_get_sync_sent(sqlite3 *db, int firstSeq, int lastSeq, int isDatapoint) {
    sqlite_int64 rowid = -1;
    int r;

    if(all_stmts[GET_SYNC_SENT_STMT]  == NULL) {
	r = sqlite3_prepare_v2(db, 
	    "SELECT key "
            "FROM SyncsSent "
            "WHERE first_seq = ? "
              "AND last_seq = ? "
              "AND is_datapoint = ? ",
	    -1, &all_stmts[GET_SYNC_SENT_STMT] , NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s SQL prepare error: %s", __FUNCTION__, sqlite3_errmsg(db));
	    return -1;
	}
    }
    sqlite3_stmt * this_stmt = all_stmts[GET_SYNC_SENT_STMT];

    int param = 1;
    // first_seq
    r = sqlite3_bind_int(this_stmt, param++, firstSeq);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "firstSeq SQL bind error");
        return -1;
    }
    // last seq
    r = sqlite3_bind_int(this_stmt, param++, lastSeq);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "lastSeq SQL bind error");
        return -1;
    }
    // is Datapoint
    r = sqlite3_bind_int(this_stmt, param++, isDatapoint);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "isDatapoint SQL bind error");
        return -1;
    }

    // Only one row should every return.
    // If more, we'll take the last one, which is as good as any metric
    for(;;) {
        r = sqlite3_step(this_stmt);
        if(r == SQLITE_BUSY) {
            g_usleep(20 * 1000);
            continue;
        }
        if(r == SQLITE_ROW) {
            rowid = sqlite3_column_int64(this_stmt, 0); 
        } else {
            break;
        }
    }
    if (r != SQLITE_DONE) {
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
            "%s(): SQL query error: %s", 
            __FUNCTION__, sqlite3_errmsg(db));
    }
        

    sqlite3_reset(this_stmt);
    sqlite3_clear_bindings(this_stmt);

    return rowid;

}

sqlite_int64 db_insert_sync_sent(sqlite3 *db, int firstSeq, int lastSeq, int isDatapoint) {
    sqlite_int64 rowid = -1;
    int r;

    if(all_stmts[INSERT_SYNC_SENT_STMT]  == NULL) {
	r = sqlite3_prepare_v2(db, 
	    "INSERT "
            "INTO SyncsSent(first_seq,last_seq,is_datapoint) "
            "VALUES (?,?,?) ",
	    -1, &all_stmts[INSERT_SYNC_SENT_STMT] , NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s SQL error: %s", __FUNCTION__, sqlite3_errmsg(db));
	    return -1;
	}
    }
    sqlite3_stmt * this_stmt = all_stmts[INSERT_SYNC_SENT_STMT];

    int param = 1;
    // first_seq
    r = sqlite3_bind_int(this_stmt, param++, firstSeq);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "firstSeq SQL bind error");
        return -1;
    }
    // last seq
    r = sqlite3_bind_int(this_stmt, param++, lastSeq);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "lastSeq SQL bind error");
        return -1;
    }
    // is Datapoint
    r = sqlite3_bind_int(this_stmt, param++, isDatapoint);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "isDatapoint SQL bind error");
        return -1;
    }


    while(SQLITE_BUSY == (r = sqlite3_step(this_stmt))){
        g_usleep(20 * 1000);
    }
        
    switch (r) {
        case SQLITE_DONE:
            // Success!
            rowid = sqlite3_last_insert_rowid(db);
            break;

        case SQLITE_CONSTRAINT:
            // Row already exists. Not Cool! We should have queried it first...
            rowid = db_get_sync_sent(db, firstSeq, lastSeq, isDatapoint);
            break;

        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s SQL error: %s", 
                   __FUNCTION__,  sqlite3_errmsg(db));
            break;
    }

    sqlite3_reset(this_stmt);
    sqlite3_clear_bindings(this_stmt);

    return rowid;

}

sqlite_int64 db_record_sync(sync_sender_config_t * sync_config, int firstSeq, int lastSeq, int isDatapoint) {
    sqlite_int64 rowid = -1;
    int r;


    if(all_stmts[INSERT_SYNC_OUTSTANDING_STMT]  == NULL) {
	r = sqlite3_prepare_v2(sync_config->db, 
	    "INSERT "
            "INTO SyncsOutstanding(recipient,sync_message,timestamp_sec) "
            "VALUES (?,?,strftime('%s','now')) ",
	    -1, &all_stmts[INSERT_SYNC_OUTSTANDING_STMT] , NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s SQL error: %s", __FUNCTION__, sqlite3_errmsg(sync_config->db));
	    return -1;
	}
    }
    sqlite3_stmt * this_stmt = all_stmts[INSERT_SYNC_OUTSTANDING_STMT];

    sqlite_int64 sync_msg_rowid = db_insert_sync_sent(sync_config->db, firstSeq, lastSeq, isDatapoint);

    int param = 1;
    // recipient
    r = sqlite3_bind_int64(this_stmt, param++, sync_config->db_key);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "SQL bind error");
        return -1;
    }
    // sync msg
    r = sqlite3_bind_int64(this_stmt, param++, sync_msg_rowid);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "SQL bind error");
        return -1;
    }

    while(SQLITE_BUSY == (r = sqlite3_step(this_stmt))){
        g_usleep(20 * 1000);
    }
        
    switch (r) {
        case SQLITE_DONE:
            // Success!
            rowid = sqlite3_last_insert_rowid(sync_config->db);
            break;

        case SQLITE_CONSTRAINT:
            // Row already exists. Not Cool! We should have queried it first...
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                    "record sync error: Row already exists");
            break;

        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s SQL error: %s", 
                   __FUNCTION__,  sqlite3_errmsg(sync_config->db));
            break;
    }

    sqlite3_reset(this_stmt);
    sqlite3_clear_bindings(this_stmt);

    return rowid;

}

// Delete and SyncsSent rows that have been acknowledged by all recipients
static int db_delete_dangling_syncs(sqlite3 * db)
{
    int r;

    if(all_stmts[DELETE_SYNCS_SENT_STMT]  == NULL) {
	r = sqlite3_prepare_v2(db, 
	    "DELETE FROM SyncsSent "
            "WHERE key NOT IN "
               "(SELECT sync_message FROM SyncsOutstanding) ",
	    -1, &all_stmts[DELETE_SYNCS_SENT_STMT] , NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s SQL error: %s", __FUNCTION__, sqlite3_errmsg(db));
	    return -1;
	}
    }
    sqlite3_stmt * this_stmt = all_stmts[DELETE_SYNCS_SENT_STMT];

    while(SQLITE_BUSY == (r = sqlite3_step(this_stmt))){
        g_usleep(20 * 1000);
    }
        
    switch (r) {
        case SQLITE_DONE:
            // Success!
            break;

        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s SQL error: %s", 
                   __FUNCTION__,  sqlite3_errmsg(db));
            break;
    }

    sqlite3_reset(this_stmt);
    sqlite3_clear_bindings(this_stmt);

    return 0;
}

int db_record_sync_ack(
    sqlite3 * db,
    sqlite_int64 channid,
    int firstSeq,
    int lastSeq,
    int isDatapoint) 
{
    int r;

    if(all_stmts[DELETE_OUTSTANDING_SYNC_STMT]  == NULL) {
	r = sqlite3_prepare_v2(db, 
	    "DELETE FROM SyncsOutstanding "
            "WHERE sync_message IN "
               "(SELECT key from SyncsSent s "
                "WHERE s.first_seq = ? "
                  "AND s.last_seq = ? "
                  "AND s.is_datapoint = ? ) "
            "AND recipient = ? ",
	    -1, &all_stmts[DELETE_OUTSTANDING_SYNC_STMT] , NULL);

	if (r != SQLITE_OK) {
	    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
                "%s SQL error: %s", __FUNCTION__, sqlite3_errmsg(db));
	    return -1;
	}
    }
    sqlite3_stmt * this_stmt = all_stmts[DELETE_OUTSTANDING_SYNC_STMT];

    int param = 1;
    // first_seq
    r = sqlite3_bind_int(this_stmt, param++, firstSeq);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "SQL bind error");
        return -1;
    }
    // last_seq
    r = sqlite3_bind_int(this_stmt, param++, lastSeq);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "SQL bind error");
        return -1;
    }
    // is_datapoint
    r = sqlite3_bind_int(this_stmt, param++, isDatapoint);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "SQL bind error");
        return -1;
    }
    // recipient
    r = sqlite3_bind_int64(this_stmt, param++, channid);
    if(r != SQLITE_OK){
        g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
            "SQL bind error");
        return -1;
    }


    while(SQLITE_BUSY == (r = sqlite3_step(this_stmt))){
        g_usleep(20 * 1000);
    }
        
    switch (r) {
        case SQLITE_DONE:
            if( sqlite3_changes(db) < 1 ) {
                g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                    "%s No local record for sync ack (%ld,%d,%d,%d)",
                   __FUNCTION__, (long int)channid, firstSeq, lastSeq, isDatapoint);
            }
            // Success!
            break;

        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s SQL error: %s", 
                   __FUNCTION__,  sqlite3_errmsg(db));
            break;
    }

    sqlite3_reset(this_stmt);
    sqlite3_clear_bindings(this_stmt);

    db_delete_dangling_syncs(db);

    return 0;
}
