
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "config.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include <sys/time.h>

#include "bionet-asn.h"
#include "bionet-data-manager.h"
#include "bdm-db.h"

int sync_message_is_ack(BDM_Sync_Message_t * sync_message)
{
    switch(sync_message->data.present) {
        case BDM_Sync_Data_PR_metadataMessage:
        case BDM_Sync_Data_PR_datapointsMessage:
            return 0;

        case BDM_Sync_Data_PR_ackMetadata:
        case BDM_Sync_Data_PR_ackDatapoints:
            return 1;

        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                  "%s(): unknown Sync Message choice", __FUNCTION__);
            return -1;
    }
}

int handle_sync_msg(BDM_Sync_Message_t * sync_message)
{
    int rc = -1;

    switch(sync_message->data.present) {
        case BDM_Sync_Data_PR_metadataMessage:
        {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
                  "sync_receive_ion(): receive Sync Metadata Message");
            rc = handle_sync_metadata_message(
                    &sync_message->data.choice.metadataMessage,
                    sync_message->syncchannel);

            break;
        }
        case BDM_Sync_Data_PR_datapointsMessage:
        {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
                  "sync_receive_ion(): receive Sync Datapoints Message");
            rc = handle_sync_datapoints_message(
                    &sync_message->data.choice.datapointsMessage, 
                    sync_message->syncchannel);

            break;
        }

        case BDM_Sync_Data_PR_ackMetadata:
        {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
                  "sync_receive_ion(): receive Ack Sync Metadata Message");
            rc = handle_sync_metadata_ack_message(
                    sync_message->data.choice.ackMetadata,
                    sync_message->syncchannel,
                    sync_message->firstSeq,
                    sync_message->lastSeq);

            break;
        }

        case BDM_Sync_Data_PR_ackDatapoints:
        {
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
                  "sync_receive_ion(): receive Ack Sync Datapoints Message");
            rc = handle_sync_datapoints_ack_message(
                    sync_message->data.choice.ackDatapoints,
                    sync_message->syncchannel,
                    sync_message->firstSeq,
                    sync_message->lastSeq);

            break;
        }

        default:
            g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                  "sync_receive_ion(): unknown Sync Message choice");
            rc = -1;
            break;
    }

    return rc;
}

int handle_sync_datapoints_ack_message(
        int code,
        sqlite_int64 channid,
        int firstSeq,
        int lastSeq)
{
    int r;
    r = db_record_sync_ack(main_db, channid, firstSeq, lastSeq, 1);

    return r;
}

int handle_sync_metadata_ack_message(
        int code,
        sqlite_int64 channid,
        int firstSeq, 
        int lastSeq)
{
    int r;
    r = db_record_sync_ack(main_db, channid, firstSeq, lastSeq, 0);

    return r;
}
