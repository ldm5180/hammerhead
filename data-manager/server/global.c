
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


#include "bionet-data-manager.h"

void * libbdm_cal_handle = NULL;
bionet_bdm_t * this_bdm = NULL;

int bdm_shutdown_now = 0;

sqlite3 * main_db = NULL;
bdm_db_batch_t * dbb = NULL;

