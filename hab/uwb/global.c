
//
// Copyright (C) 2008-2009, Gary Grobe and the Regents of the University of Colorado.
//

#include <glib.h>
#include <netinet/in.h>
#include "uwb.h"

char *ip = NULL;

char *node_id;

int port = 9400;
int timeout = 10;

double scan_data[80000];
short signed int complete_scan[COMPLETE_SCAN_SIZE];
long current_range_count = 0;
long scan_points = 0;

struct sockaddr_in uwb_address;



