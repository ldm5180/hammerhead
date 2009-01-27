
//
// Copyright (C) 2008-2009, Gary Grobe and the Regents of the University of Colorado.
//


#include "ltkc.h"
#include "hardware-abstractor.h"


GMainLoop *main_loop = NULL;

bionet_hab_t *hab = NULL;
bionet_node_t *reader_node = NULL;

int scans_left_to_do = 0;

int show_messages = 0;
int gpi_delay = 0;
int gpi_polarity = 1;
int num_scans = 1;
int scan_idle = 3000;
int scan_timeout = 5000;

LLRP_tSConnection *pConn;
LLRP_tSTypeRegistry *pTypeRegistry;

