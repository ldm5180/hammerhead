
//
// Copyright (C) 2008-2009, Gary Grobe and the Regents of the University of Colorado.
//

#ifndef __TD_RADIO_H_
#define __TD_RADIO_H_

#include <stdbool.h>

#define NO_SCAN			0
#define WRONG_SEGMENT	1
#define NEXT_SEGMENT	2
#define SCAN_COMPLETE	3

#define SCAN_SIZE			2500
#define COMPLETE_SCAN_SIZE	32767
#define MAX_SAVE_SCANS		1000
#define NO_LOCK				1

// Todo: why was this size chosen ?
#define BUFFER_SZ			32767 //20000

#include "hardware-abstractor.h"

extern double scan_data[];
extern short signed int complete_scan[];
extern long current_range_count;
extern long scan_points;

extern bionet_hab_t *uwb_hab;

extern char *node_id;
extern int port;
extern int timeout;

extern struct sockaddr_in uwb_address;

int uwb_connect(const int uwb_port);
int uwb_read(int fd, char* buffer);
void node_remove();

#endif 

