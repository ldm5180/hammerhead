
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "bionet-data-manager.h"
#include "bdm-util.h"

void handle_sync_datapoints_message(client_t *client, BDM_Sync_Datapoints_Message_t *message) {
    int sri;

    for (sri = 0; sri < message->list.count; sri++) {
	BDMSyncRecord_t * sync_record;
	const char * bdm_id;
	int rri;

	//get the sync record
	sync_record = message->list.array[sri];
	
	//get the BDM-ID from the sync record
	bdm_id = (const char *)sync_record->bdmID.buf;
	g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
	      "Received sync record from BDM: %s", bdm_id);

	//process every resource
	for (rri = 0; rri < sync_record->syncResources.list.count; rri++) {
	    ResourceRecord_t * resource_rec;
	    uint8_t resource_key[BDM_RESOURCE_KEY_LENGTH];
	    int dpi;
	    
	    resource_rec = sync_record->syncResources.list.array[rri];
	    
	    memcpy(resource_key, resource_rec->resourceKey.buf, BDM_RESOURCE_KEY_LENGTH);
	    
	    for (dpi = 0; dpi < resource_rec->resourceDatapoints.list.count; dpi++) {
		Datapoint_t *dp;
                bdm_datapoint_t bdmdp;

                bdmdp.bdm_id = (char*)bdm_id;

		dp = resource_rec->resourceDatapoints.list.array[dpi];
		
		switch(dp->value.present) {

		case Value_PR_binary_v:
		    bdmdp.type = DB_INT;
		    bdmdp.value.i = dp->value.choice.binary_v;
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
			  "    handle_sync_datapoints_message(): binary: %d", 
                          bdmdp.value.i);
		    break;

		case Value_PR_uint8_v:
		    bdmdp.type = DB_INT;
		    bdmdp.value.i = dp->value.choice.uint8_v;
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
			  "    handle_sync_datapoints_message(): uint8_t: %u", 
                          (uint8_t)bdmdp.value.i);
		    break;

		case Value_PR_int8_v:
		    bdmdp.type = DB_INT;
		    bdmdp.value.i = dp->value.choice.int8_v;
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
			  "    handle_sync_datapoints_message(): int8_t: %d",
                          (int8_t)bdmdp.value.i);
		    break;

		case Value_PR_uint16_v:
		    bdmdp.type = DB_INT;
		    bdmdp.value.i = dp->value.choice.uint16_v;
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
			  "    handle_sync_datapoints_message(): uint16_t: %u",
                          (uint16_t)bdmdp.value.i);
		    break;

		case Value_PR_int16_v:
		    bdmdp.type = DB_INT;
		    bdmdp.value.i = dp->value.choice.int16_v;
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
			  "    handle_sync_datapoints_message(): int16_t: %d",
                          (int16_t)bdmdp.value.i);
		    break;

		case Value_PR_uint32_v:
		{
		    int r;
		    long l;
		    bdmdp.type = DB_INT;
		    r = asn_INTEGER2long(&dp->value.choice.uint32_v, &l);
		    if (r != 0) {
			g_log(BIONET_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
			      "bionet_asn_to_datapoint(): error converting ASN INTEGER to native Datapoint value");
		    }
		    bdmdp.value.i = (uint32_t)l;
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
			  "    handle_sync_datapoints_message(): uint32_t: %u", 
                          (uint32_t)l);
		    break;
		}

		case Value_PR_int32_v:
		    bdmdp.type = DB_INT;
		    bdmdp.value.i = dp->value.choice.int32_v;
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
			  "    handle_sync_datapoints_message(): int32_t: %d",
                          (int32_t)bdmdp.value.i);
		    break;

		case Value_PR_real:
		    bdmdp.type = DB_DOUBLE;
		    bdmdp.value.d = dp->value.choice.real;
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
			  "    handle_sync_datapoints_message(): double: %g", 
                          bdmdp.value.d);
		    break;

		case Value_PR_string:
		    bdmdp.type = DB_STRING;
		    bdmdp.value.str = (char*)dp->value.choice.string.buf;
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_INFO,
			  "    handle_sync_datapoints_message(): string: %s", 
                          bdmdp.value.str);
		    break;

		default:
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
			  "Unknown datatype in BDM Sync Datapoint Message, ignoring datapoint");
		    continue;
		} /* switch(dp->value.present) */

		if (0 != bionet_GeneralizedTime_to_timeval(&dp->timestamp, &bdmdp.timestamp)) {
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
			  "error converting GeneralizedTime '%s' to struct timeval: %m",
			  dp->timestamp.buf);
		    return;  // FIXME: return an error message to the client
		}

		if (db_add_datapoint_sync(main_db, resource_key, &bdmdp)) {
		    g_log(BDM_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
			  "Failed adding datapoint to DB.");
		}
	    }
	}
    }

}

// Emacs cruft
// Local Variables:
// mode: C
// c-file-style: "Stroustrup"
// End:
