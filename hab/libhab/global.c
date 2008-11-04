
//
// Copyright (C) 2008, Regents of the University of Colorado.
//

#include <stdio.h>

#include "bionet-util.h"

#include "libhab-internal.h"
#include "hardware-abstractor.h"




int libhab_cal_fd = -1;




#if 0

GSList *libhab_queued_messages_from_nag = NULL;


char *libhab_nag_hostname = NULL;
unsigned short libhab_nag_port = 11001;

int libhab_nag_timeout = 5;

char *libhab_type = NULL;
char *libhab_id = NULL;


bionet_nxio_t *libhab_nag_nxio = NULL;


char *libhab_nag_error = NULL;


void (*libhab_callback_set_resource)(const char *node_id, const char *resource_id, const char *value) = NULL;

#endif

