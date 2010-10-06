// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#ifndef BDM_STATS_H
#define BDM_STATS_H

#include "bionet-data-manager.h"
#include "bdm-db.h"
#include "config.h"
#include "hardware-abstractor.h"
#include <glib.h>

extern unsigned int bdm_stats_interval;
bionet_hab_t * start_stat_hab(const char * bdm_id, int *pHab_fd);
gboolean update_stat_hab(gpointer usr_data);

// Global BDM Stat settings
extern bionet_hab_t * bdm_hab;
extern int start_hab;
extern int ignore_self;


// Global BDM Statistics
extern struct timeval dp_ts_accum;
extern struct timeval db_accum;

extern uint32_t num_bionet_datapoints;
extern uint32_t num_sync_datapoints;
extern uint32_t num_db_commits;

extern uint32_t num_bionet_events;
extern uint32_t num_sync_sent_events;
extern uint32_t num_sync_recv_events;

extern uint32_t num_syncs_sent;
extern uint32_t num_syncs_recvd;
extern uint32_t num_sync_acks_sent;
extern uint32_t num_sync_acks_recvd;

#endif // BDM_STATS_H
